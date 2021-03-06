// OpenGUI (http://opengui.sourceforge.net)
// This source code is released under the BSD License
// See LICENSE.TXT for details

#ifndef BE0429DB_7CC6_424c_8AF8_96B0BE08270D
#define BE0429DB_7CC6_424c_8AF8_96B0BE08270D

#include "OpenGUI_PreRequisites.h"
#include "OpenGUI_Exports.h"
#include "OpenGUI_String.h"
#include "OpenGUI_Types.h"
#include "OpenGUI_RefObject.h"

namespace OpenGUI {

	class TextureDataRect;
	struct FontGlyph;
	struct FontGlyphMetrics;
	class Resource;

	//! Used to load fonts and render glyphs from those fonts into memory segments.
	class OPENGUI_API FontSet: public RefObject {
		friend class FontManager;
	protected:
		//! Font creation implies loading of the font via FreeType.
		/*! \param sourceFilename filename of the font to load
		\param fontName the friendly name for this FontSet */
		FontSet( const String& sourceFilename, const String& fontName );
		//! Destruction also unloads the font from FreeType
		~FontSet();
		virtual void finalize();
	public:
		//! Returns the filename used to load the font
		const String& getFilename() {
			return mFilename;
		}

		//! Returns the name of this font
		const String& getName() {
			return mFontName;
		}

		//! Retrieves the FontGlyph struct for a glyph \a glyph_charCode at the given \a pixelSize.
		/*!
		\return \c true on success, \c false if the glyph cannot be generated because of some FreeType error
		\param[in] glyph_charCode The Unicode character code of the glyph you are requesting
		\param[in] pixelSize The size of the glyph you'd like, in pixels. Translation to/from pixels is up to
		the user. (See: Screen::getPPU(), Screen::getUPI())
		\param[out] outFontGlyph On success, this is initialized with information about the glyph.
		*/
		bool getGlyph( const Char glyph_charCode, const IVector2& pixelSize, FontGlyph& outFontGlyph );

		//! Returns the line height in pixels for a given \c pointSize
		unsigned int getLineSpacing( unsigned int pointSize );

		//! Returns the font wide ascender in pixels for a given \c pointSize
		int getAscender( unsigned int pointSize );

		//! Returns the font wide descender in pixels for a given \c pointSize
		int getDescender( unsigned int pointSize );

		//! Returns the maximum horizontal advance for all characters at the given \c pointSize
		int getMaxAdvance( unsigned int pointSize );

		//! Returns the true width for the given \c text at the given \c pointSize
		int getTextWidth( const IVector2& pixelSize, const String& text );

		//! Renders the requested glyph to the given TextureDataRect in the size specified.
		/*!
		\note The TextureDataRect will be resized to exactly hold the glyph. \a destGlyphMetrics will
		contain the glyph metrics in pixels for the glyph at the requested size
		\remarks This function is used by the FontCache to generate glyphs for storage in FontAtlases
		so that they can be renderer to the screen.
		\see Most users are probably looking for BrushText, or at the most getGlyph().
		*/
		void renderGlyph( const Char glyph_charCode, const IVector2& pixelSize, TextureDataRect* destTDR, FontGlyphMetrics& destGlyphMetrics );

	private:
		void* mFT_Face;
		String mFilename;
		String mFontName;
		Resource *mFontResource;
	};
	typedef RefObjHandle<FontSet> FontSetPtr;

} // namespace OpenGUI{

#endif // BE0429DB_7CC6_424c_8AF8_96B0BE08270D
