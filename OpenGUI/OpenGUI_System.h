// OpenGUI (http://opengui.sourceforge.net)
// This source code is released under the BSD License
// See LICENSE.TXT for details

#ifndef DC3E54A4_1432_4fa1_A635_D76797D7ED89
#define DC3E54A4_1432_4fa1_A635_D76797D7ED89

#include "OpenGUI_PreRequisites.h"
#include "OpenGUI_Exports.h"
#include "OpenGUI_String.h"
#include "OpenGUI_Singleton.h"
#include "OpenGUI_LogSystem.h"
#include "OpenGUI_Exception.h"
#include "OpenGUI_Math.h"
#include "OpenGUI_Types.h"
#include "OpenGUI_XML.h"

//! All %OpenGUI classes are contained under the "OpenGUI" namespace
namespace OpenGUI {
	//forward declarations

	//class WidgetFactoryManager;
	//class WidgetTemplateManager;

	class TimerManager;
	class LogManager;
	class LogListener;
	class LogListenerToFile;
	class CursorManager;
	class ResourceProvider;
	class FontManager;
	class PluginManager;
	class Renderer;
	class TextureManager;
	class ImageryManager;
	class ScreenManager;
	class CursorManager;
	class WidgetManager;
	class FormManager;
	class XMLParser;

	/*!
		\brief
		The System object is the base of the OpenGUI project. It is responsible for
		initializing and destroying the various subsystems, and provides some functions
		to perform global activities, such as loading plugins and performing mass updates
		for all Screen objects.
		\n \b [Singleton]

		The construction and destruction of this class marks the initialization and
		shutdown of %OpenGUI as a whole. You must have a valid System object before you
		can use most other subsystems of %OpenGUI, and destruction of the System object will
		cause the destruction of all subsystems as well.

		This class is implemented using the Singleton system. There can only be one System
		object instantiated at any point in time.
	*/
	class OPENGUI_API System : public Singleton<System> {
	public:
		//Reimplementation required for this style of singleton implementation to work across DLLs
		//! Retrieve the current singleton, if one exists. If none exists, this will cause an error.
		static System& getSingleton( void );

		//Reimplementation required for this style of singleton implementation to work across DLLs
		//! Retrieve a pointer to the current singleton, if one exists. If none exists, this will return 0.
		static System* getSingletonPtr( void );

		//! The constructor for the System object.
		/*! Firstly, the System object is a singleton, so only one can exist
			in an application at any given point in time.

			In order to create an instance of the System object, you must
			provide a pointer to a valid Renderer. The resourceProvider pointer
			is optional, as a generic implementation of that comes pre-built
			with the library.

			\param renderer pointer to a valid Renderer object
			\param resourceProvider pointer to a valid ResourceProvider object, or 0
				to use the built in generic ResourceProvider
			\param logFile name of the log file to write log message to. If "" is used,
				no log file will be created
		*/
		System( Renderer* renderer, ResourceProvider* resourceProvider = NULL, const String& logFile = "OpenGUI.log" );
		/*! As the other constructor except a log listener is passed in place of a log filename.
			\param renderer pointer to a valid Renderer object
			\param resourceProvider pointer to a valid ResourceProvider object, or 0
				to use the built in generic ResourceProvider
			\param logListener A valid pointer to a LogListener, or 0 for no logging.
		*/
		System( Renderer* renderer, ResourceProvider* resourceProvider, LogListener* logListener );

		~System();

		//! loads a plugin by filename
		void loadPlugin( String filename );
		//! unloads a plugin by filename
		void unloadPlugin( String filename );


		//! Updates all subsystems in the necessary order
		void update();

		//! Updates all auto updating properties of all Screens
		void updateScreens();

		//! Updates the TimerManager using the built in time advancement code
		void updateTime();

		//! Returns a pointer to the registered ResourceProvider. Apps really shouldn't need, or use, this.
		/*! \deprecated Ideally, no part of %OpenGUI should need this. */
		ResourceProvider* _getResourceProvider() {
			return mResourceProvider;
		}

	protected:

	private:
		// This is the actual constructor, the other 2 both call this one after getting the logs up and running.
		void doConstructor( Renderer* renderer, ResourceProvider* resourceProvider );

		// XML tag handlers for <OpenGUI> root tag
		static bool _OpenGUI_XMLNode_Load( const XMLNode& node, const String& nodePath );
		static bool _OpenGUI_XMLNode_Unload( const XMLNode& node, const String& nodePath );

		//Logging Facilities
		LogManager* m_LogManager;
		LogListenerToFile* mDefaultLogListener;

		//Font Manager
		FontManager* mFontManager;

		//Plugin Manager
		PluginManager* m_PluginManager;

		//CursorManager
		CursorManager* mCursorManager;

		//WidgetManager
		WidgetManager* mWidgetManager;

		//FormManager
		FormManager* mFormManager;

		//TimerManager
		TimerManager* mTimerManager;
		bool m_PerformAutoTicks;

		//Resource Provider Related Members
		ResourceProvider* mResourceProvider; //pointer to the resource provider
		bool mUsingGenericResourceProvider; //if we're using the generic resource provider, we are responsible for the delete

		//Renderer Related Members
		Renderer* mRenderer;

		//TextureManager Related Members
		TextureManager* mTextureManager;

		//ImageryManager Related Members
		ImageryManager* mImageryManager; //root pointer to the created ImageryManager singleton.

		//ScreenManager
		ScreenManager* mScreenManager;

		//XMLParser
		XMLParser* mXMLParser;
	};
};

#endif
