// OpenGUI (http://opengui.sourceforge.net)
// This source code is released under the BSD License
// See LICENSE.TXT for details

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include "GL/glfw.h"
#include "OGL_FBO.h"

#include "corona.h"

#include "Renderer_OpenGL.h"
#include "OGL_Texture.h"



namespace OpenGUI {
	//###########################################################
	Renderer_OpenGL::Renderer_OpenGL( int initial_width, int initial_height ) {
		mDefaultViewport.setSize( IVector2( initial_width, initial_height ) );
		mCurrentContext = 0;
		mCurrentViewport = 0;
		mInGLBegin = false;
		mInRender = false;
		mCurrentTextureState = 0;

		mSupportRTT = InitializeFBO();
		if ( glfwExtensionSupported( "GL_EXT_texture_rectangle" ) || glfwExtensionSupported( "GL_ARB_texture_rectangle" ) )
			mSupportRectTex = true;
		else
			mSupportRectTex = false;
	}
	//###########################################################
	Renderer_OpenGL::~Renderer_OpenGL() {
		/**/
	}
	//###########################################################
	void Renderer_OpenGL::setDim( int w, int h ) {
		mDefaultViewport.setSize( IVector2( w, h ) );
	}
	//###########################################################
	Viewport* Renderer_OpenGL::getDefaultViewport() {
		return &mDefaultViewport;
	}
	//###########################################################
	Viewport* Renderer_OpenGL::createRTTViewport( const IVector2& size ) {
		if ( !supportsRenderToTexture() )
			return 0;
		return new OGL_RTT_Viewport( size );
	}
	//###########################################################
	void Renderer_OpenGL::destroyRTTViewport( Viewport* viewport ) {
		delete static_cast<OGL_RTT_Viewport*>( viewport );
	}
	//###########################################################
	void Renderer_OpenGL::drawTriangles( const TriangleList& triangles, float xScaleUV, float yScaleUV ) {
		safeBegin();
		for ( TriangleList::const_iterator iter = triangles.begin();
				iter != triangles.end(); iter++ ) {
			const Triangle& t = ( *iter );
			for ( int i = 0; i < 3; i++ ) {
				glColor4f(	t.vertex[i].color.Red,
						   t.vertex[i].color.Green,
						   t.vertex[i].color.Blue,
						   t.vertex[i].color.Alpha );
				glTexCoord2f( t.vertex[i].textureUV.x * xScaleUV, t.vertex[i].textureUV.y * yScaleUV );
				glVertex3f( t.vertex[i].position.x, t.vertex[i].position.y, 0.0f );
			}
		}
	}
	//###########################################################
	void Renderer_OpenGL::drawTriangles( const TriangleList& triangles ) {
		safeBegin();
		for ( TriangleList::const_iterator iter = triangles.begin();
				iter != triangles.end(); iter++ ) {
			const Triangle& t = ( *iter );
			for ( int i = 0; i < 3; i++ ) {
				glColor4f(	t.vertex[i].color.Red,
						   t.vertex[i].color.Green,
						   t.vertex[i].color.Blue,
						   t.vertex[i].color.Alpha );
				glTexCoord2f( t.vertex[i].textureUV.x, t.vertex[i].textureUV.y );
				glVertex3f( t.vertex[i].position.x, t.vertex[i].position.y, 0.0f );
			}
		}
	}
	//###########################################################
	void Renderer_OpenGL::safeBegin() {
		if ( mInGLBegin ) return;
		glBegin( GL_TRIANGLES );
		mInGLBegin = true;
	}
	//###########################################################
	void Renderer_OpenGL::safeEnd() {
		if ( !mInGLBegin ) return;
		glEnd();
		mInGLBegin = false;
	}
	//###########################################################
	void Renderer_OpenGL::selectViewport( Viewport* activeViewport ) {
		if ( mInRender )
			OG_THROW( Exception::ERR_INTERNAL_ERROR, "Requested Viewport switch inside render markers", __FUNCTION__ );
		if ( activeViewport == 0 )
			OG_THROW( Exception::ERR_INVALIDPARAMS, "Bad Viewport: 0", __FUNCTION__ );
		mCurrentViewport = static_cast<OGL_Viewport*>( activeViewport );
	}
	//###########################################################
	void Renderer_OpenGL::selectTextureState( Texture* texture ) {
		if ( texture == mCurrentTextureState ) return; //skip if we can
		mCurrentTextureState = texture;
		safeEnd();

		if ( !texture ) {
			//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			if ( mSupportRectTex ) glDisable( GL_TEXTURE_RECTANGLE_ARB );
			glBindTexture( GL_TEXTURE_2D, 0 );
			return;
		}

		if ( texture->isRenderTexture() ) {
			OGLRTexture* tex = static_cast<OGLRTexture*>( texture );
			//glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

			if ( mSupportRectTex ) {
				glEnable( GL_TEXTURE_RECTANGLE_ARB );
				glBindTexture( GL_TEXTURE_RECTANGLE_ARB, tex->textureId );
			} else {
				glBindTexture( GL_TEXTURE_2D, tex->textureId );
			}
		} else {
			OGLTexture* tex = static_cast<OGLTexture*>( texture );
			//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			if ( mSupportRectTex ) glDisable( GL_TEXTURE_RECTANGLE_ARB );
			glBindTexture( GL_TEXTURE_2D, tex->textureId );
		}
	}
	//###########################################################
	void Renderer_OpenGL::doRenderOperation( RenderOperation& renderOp ) {
		if ( !renderOp.triangleList ) return; //abort if no data to draw
		Texture* texture = renderOp.texture.get();
		//change texture state if needed
		selectTextureState( texture );

		if ( texture && texture->isRenderTexture() && mSupportRectTex ) {
			float xUVScale;
			float yUVScale;
			const IVector2& t = static_cast<OGLRTexture*>( renderOp.texture.get() )->getSize();
			xUVScale = ( float )t.x;
			yUVScale = ( float )t.y;
			drawTriangles( *( renderOp.triangleList ), xUVScale, yUVScale );
		} else {
			drawTriangles( *( renderOp.triangleList ) );
		}
	}
	//###########################################################
	void Renderer_OpenGL::preRenderSetup() {
		if ( !mCurrentViewport )
			OG_THROW( Exception::ERR_INTERNAL_ERROR, "No valid Viewport selected", __FUNCTION__ );
		mInRender = true;
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f );


		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		glEnable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_TEXTURE_2D );
		glShadeModel( GL_SMOOTH );

		//glDisable(GL_CULL_FACE);
		glFrontFace( GL_CCW );
		glCullFace( GL_BACK );
		glEnable( GL_CULL_FACE ); //test

		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		mInGLBegin = false;
		mCurrentTextureState = 0;

		mCurrentContext = 0;
		if ( mSupportRTT ) {
			OGLRTexture* start = mCurrentViewport->getRenderTexture();
			if ( start ) {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, start->fboId );
				glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
				glClear( GL_COLOR_BUFFER_BIT );
			} else
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
		}
		glViewport( 0, 0, mCurrentViewport->getSize().x, mCurrentViewport->getSize().y );
	}
	//###########################################################
	void Renderer_OpenGL::postRenderCleanup() {
		safeEnd();
		selectTextureState( 0 );
		selectRenderContext( 0 ); // be kind, rewind
		mInRender = false;
		if ( mSupportRTT )
			glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	}
	//###########################################################
	Texture* Renderer_OpenGL::createTextureFromFile( const String& filename ) {
		safeEnd();
		selectTextureState( 0 );
		OGLTexture* retval = 0;

		//Load the image from the disk
		TextureData* td = LoadTextureData( filename );
		if ( !td ) return 0;

		retval = new OGLTexture();
		if ( !retval ) return 0;

		retval->setName( filename );

		retval->setSize( IVector2( td->getWidth(), td->getHeight() ) );

		GLint internalFormat;
		GLenum dataFormat;
		switch ( td->getBPP() ) {
		case 1:
			internalFormat = GL_ALPHA;
			dataFormat = GL_ALPHA;
			break;
		case 3:
			internalFormat = GL_RGB;
			dataFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
			break;
		}


		glGenTextures( 1, &( retval->textureId ) );
		glBindTexture( GL_TEXTURE_2D, retval->textureId );
		gluBuild2DMipmaps( GL_TEXTURE_2D, //2D texture
						   internalFormat, //destination format
						   td->getWidth(), //image width
						   td->getHeight(), //image height
						   dataFormat, //the format of the pixel data
						   GL_UNSIGNED_BYTE, //each channel consists of 1 unsigned byte
						   td->getPixelData() //pointer to the image data
						 );

		//set up texture filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

		delete td;

		glBindTexture( GL_TEXTURE_2D, 0 );

		return retval;
	}
	//###########################################################
	Texture* Renderer_OpenGL::createTextureFromTextureData( const TextureData *textureData ) {
		safeEnd();
		selectTextureState( 0 );
		const TextureData* td = textureData; // copy/paste quick fix
		OGLTexture* retval = 0;
		retval = new OGLTexture( );
		if ( !retval ) return 0;

		retval->setName( "__## TextureFromMemory ##__" );

		retval->setSize( IVector2( td->getWidth(), td->getHeight() ) );

		GLint internalFormat;
		GLenum dataFormat;
		switch ( td->getBPP() ) {
		case 1:
			internalFormat = GL_ALPHA;
			dataFormat = GL_ALPHA;
			break;
		case 3:
			internalFormat = GL_RGB;
			dataFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA;
			dataFormat = GL_RGBA;
			break;
		}


		glGenTextures( 1, &( retval->textureId ) );
		glBindTexture( GL_TEXTURE_2D, retval->textureId );
		glTexImage2D(	GL_TEXTURE_2D, //2D texture
					  0, //mipmap level 0
					  internalFormat, // the texture format
					  td->getWidth(), //image width
					  td->getHeight(), //image height
					  0, //no border (does anyone ever use this?)
					  dataFormat, //the format of the pixel data
					  GL_UNSIGNED_BYTE, //each channel consists of 1 unsigned byte
					  td->getPixelData() //pointer to the image data
					);

		//set up texture filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		return retval;
	}
	//###########################################################
	void Renderer_OpenGL::updateTextureFromTextureData( Texture* texture, const TextureData *textureData ) {
		safeEnd();
		selectTextureState( 0 );
		const TextureData* td = textureData; // copy/paste quick fix
		OGLTexture* retval = 0;
		retval = ( OGLTexture* ) texture;
		if ( !retval ) return;

		//throw away old data
		glDeleteTextures( 1, &( retval->textureId ) );

		retval->setName( "__## TextureFromMemory ##__" );

		retval->setSize( IVector2( td->getWidth(), td->getHeight() ) );

		GLint internalFormat;
		GLenum dataFormat;
		switch ( td->getBPP() ) {
		case 1:
			internalFormat = GL_ALPHA;
			dataFormat = GL_ALPHA;
			break;
		case 3:
			internalFormat = GL_RGB;
			dataFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA;
			dataFormat = GL_RGBA;
			break;
		}


		glGenTextures( 1, &( retval->textureId ) );
		glBindTexture( GL_TEXTURE_2D, retval->textureId );
		glTexImage2D(	GL_TEXTURE_2D, //2D texture
					  0, //mipmap level 0
					  internalFormat, // the texture format
					  td->getWidth(), //image width
					  td->getHeight(), //image height
					  0, //no border (does anyone ever use this?)
					  dataFormat, //the format of the pixel data
					  GL_UNSIGNED_BYTE, //each channel consists of 1 unsigned byte
					  td->getPixelData() //pointer to the image data
					);

		//set up texture filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	//###########################################################
	void Renderer_OpenGL::destroyTexture( Texture* texturePtr ) {
		if ( !texturePtr ) return;
		safeEnd();

		OGLTexture* texptr = dynamic_cast<OGLTexture*>( texturePtr );
		if ( texptr ) {
			if ( texptr->textureId ) {
				glDeleteTextures( 1, &( texptr->textureId ) );
			}
			delete texptr;
			return;
		}
	}
	//###########################################################
	TextureData* Renderer_OpenGL::LoadTextureData( String filename ) {
		//we can't load anything until the system is up
		//but we should try to play nice
		if ( !System::getSingletonPtr() )
			return 0;
		ResourceProvider* rp = System::getSingletonPtr()->_getResourceProvider();
		if ( rp == 0 ) return 0;

		//load the resource into memory via the registered resource provider
		Resource resource;
		try {
			rp->loadResource( filename, resource );
		} catch ( Exception e ) {
			return 0;
		};

		//pass the data to corona
		corona::File* cf = corona::CreateMemoryFile( resource.getData(), ( int )resource.getSize() );
		resource.release(); //at this point, we don't need the resource loaded any longer
		if ( !cf ) {
			return 0;
		}
		corona::Image* img = corona::OpenImage( cf, corona::PF_DONTCARE );
		delete cf;
		cf = 0; //at this point, we don't need the memory file any longer

		TextureData* retval = new TextureData();
		int imgHeight, imgWidth, imgBPP;
		imgHeight = img->getHeight();
		imgWidth = img->getWidth();

		//perform data format enumeration and
		switch ( img->getFormat() ) {
		case corona::PF_I8:
			imgBPP = 1;
			break;

		case corona::PF_B8G8R8A8:
			img = corona::ConvertImage( img, corona::PF_R8G8B8A8 );
			if ( img == 0 ) {//we failed to convert, bail out
				delete retval;
				return 0;
			}
		case corona::PF_R8G8B8A8:
			imgBPP = 4;
			break;

		case corona::PF_B8G8R8:
			img = corona::ConvertImage( img, corona::PF_R8G8B8 );
			if ( img == 0 ) {//we failed to convert, bail out
				delete retval;
				return 0;
			}
		case corona::PF_R8G8B8:
			imgBPP = 3;
			break;

		default:
			delete retval;
			delete img;
			return 0;
			break;//<- habit
		}

		/*	At this point, the data should now be completely loaded into a known format.
		All we need to do is copy the img buffer contents and we're done
		*/
		retval->setData( imgWidth, imgHeight, imgBPP, img->getPixels() );

		delete img; //and delete this or we leak
		return retval;
	}
	//#####################################################

	//#####################################################
	//#####################################################
	// RENDER TO TEXTURE SUPPORT FUNCTIONS
	//#####################################################
	//#####################################################
	bool Renderer_OpenGL::supportsRenderToTexture() {
		return mSupportRTT;
	}
	//#####################################################
	void Renderer_OpenGL::selectRenderContext( RenderTexture* context ) {
		if ( mCurrentContext != context ) {
			safeEnd();

			OGLRTexture* rtex = 0;
			mCurrentContext = context;

			if ( mCurrentContext ) {
				rtex = static_cast<OGLRTexture*>( mCurrentContext );
			} else {
				if ( mCurrentViewport )
					rtex = mCurrentViewport->getRenderTexture();
			}

			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			if ( rtex ) {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, rtex->fboId );
				glViewport( 0, 0, rtex->getSize().x, rtex->getSize().y );
			} else {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
				glViewport( 0, 0, mDefaultViewport.getSize().x, mDefaultViewport.getSize().y );
			}
		}
	}
	//#####################################################
	void Renderer_OpenGL::clearContents() {
		if ( 0 == mCurrentContext ) return; // don't clear the main viewport
		safeEnd();
		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glClear( GL_COLOR_BUFFER_BIT );
	}
	//#####################################################
	RenderTexture* Renderer_OpenGL::createRenderTexture( const IVector2& size ) {
		safeEnd();
		selectTextureState( 0 );
		OGLRTexture* ret = new OGLRTexture();
		ret->setSize( size );
		ret->setName( "__RenderTexture__" );
		GLuint textid;

		glGenTextures( 1, &textid );
		ret->textureId = textid;

		// switch rendering context so we can set things up
		RenderTexture* prevContext = mCurrentContext;
		selectRenderContext( ret );

		if ( mSupportRectTex ) {
			ret->maxUVs = FVector2(( float )ret->getSize().x, ( float )ret->getSize().y );

			glBindTexture( GL_TEXTURE_RECTANGLE_ARB, textid );
			glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, //2D rectangle texture
						  0, //mipmap level 0
						  GL_RGBA, // texture format
						  ret->getSize().x, // texture width
						  ret->getSize().y, // texture height
						  0, // no border
						  GL_RGBA, // input data format
						  GL_UNSIGNED_INT, // input data channel size
						  NULL ); // this is a blank texture, no input data

			if ( glGetError() == GL_INVALID_VALUE )
				OG_THROW( Exception::ERR_INTERNAL_ERROR, "Something broke with rectangle textures", __FUNCTION__ );

			// attach the texture to the frame buffer as the color destination
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, //target
									   GL_COLOR_ATTACHMENT0_EXT, // attachment destination
									   GL_TEXTURE_RECTANGLE_ARB, // destination attachment object type
									   textid, // destination attachment object ID
									   0 ); // level within attaching object
		}
		if ( !mSupportRectTex ) {
			ret->maxUVs = FVector2( 1.0f, 1.0f );

			glBindTexture( GL_TEXTURE_2D, textid );

			// set up texture filtering
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			// create the blank texture
			glTexImage2D( GL_TEXTURE_2D, //2D texture
						  0, //mipmap level 0
						  GL_RGBA8, // texture format
						  ret->getSize().x, // texture width
						  ret->getSize().y, // texture height
						  0, // no border
						  GL_RGBA, // input data format
						  GL_UNSIGNED_BYTE, // input data channel size
						  NULL ); // this is a blank texture, no input data

			//!\todo Add proper handling when non power of 2 textures are not supported
			if ( glGetError() == GL_INVALID_VALUE )
				OG_THROW( Exception::ERR_INTERNAL_ERROR, "I don't support non power of 2 textures", __FUNCTION__ );

			// attach the texture to the frame buffer as the color destination
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, //target
									   GL_COLOR_ATTACHMENT0_EXT, // attachment destination
									   GL_TEXTURE_2D, // destination attachment object type
									   textid, // destination attachment object ID
									   0 ); // level within attaching object
		}


		//restore the previous render context
		selectRenderContext( prevContext );

		return ret;
	}
	//#####################################################
	void Renderer_OpenGL::destroyRenderTexture( RenderTexture* texturePtr ) {
		if ( !texturePtr ) return;
		safeEnd();

		if ( mCurrentContext ==  texturePtr ) // never delete the current context
			selectRenderContext( 0 ); // so we switch back to the default context if needed

		OGLRTexture* rtexptr = dynamic_cast<OGLRTexture*>( texturePtr );
		if ( rtexptr ) {
			if ( rtexptr->textureId ) {
				glDeleteTextures( 1, &( rtexptr->textureId ) );
			}
			delete rtexptr;
			return;
		}
	}
	//#####################################################
}
;//namespace OpenGUI{





