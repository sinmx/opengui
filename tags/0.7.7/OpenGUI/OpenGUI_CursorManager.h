#ifndef E1334B40_05A0_4a30_9714_34D7E9AF7378
#define E1334B40_05A0_4a30_9714_34D7E9AF7378

#include "OpenGUI_PreRequisites.h"

#include "OpenGUI_Singleton.h"
#include "OpenGUI_Exports.h"
#include "OpenGUI_Cursor.h"


namespace OpenGUI{

	//! The CursorManager provides cursor registration and retrieval services.
	class OPENGUI_API CursorManager : public Singleton<CursorManager>
	{
		friend class Cursor;
		friend class System;
	public:
		//! Constructor. This singleton is automatically created by System when needed. Do not create yourself.
		CursorManager();
		//! Destructor
		~CursorManager();

		//Reimplementation required for this style of singleton implementation to work across DLLs
		//! Retrieve the current singleton, if one exists. If none exists, this will cause an error.
		static CursorManager& getSingleton(void);

		//Reimplementation required for this style of singleton implementation to work across DLLs
		//! Retrieve a pointer to the current singleton, if one exists. If none exists, this will return 0.
		static CursorManager* getSingletonPtr(void);

		/*! \brief Adds a cursor to the CursorManager
			Cursor names must be unique. Adding a cursor with the same name
			as a previously existing cursor will cause the automatic destruction
			of the existing cursor, and the newly added cursor will be used as a
			replacement.
			\note The CursorManager will take over responsibility of the memory
			associated with this pointer. All registered 
		*/
		void addCursor(Cursor* cursorPtr, std::string name);

		//! Retrieves a previously added cursor by name. Returns 0 if cursor could not be found.
		Cursor* getCursor(std::string name);

		//! Destroys a previously registered cursor.
		/*! \note Performs delete operation on the stored cursor pointer.
		*/
		void destroyCursor(std::string name);

		//! Destroys a previously registered cursor.
		/*! \see void destroyCursor(std::string name); */
		void destroyCursor(Cursor* cursorPtr);

		//! Unregisters a cursor without deleting the object.
		/*! If for some reason you want to manually remove a previously
			registered cursor without deleting the object itself, this
			is the function you are looking for.
		*/
		void removeCursor(Cursor* cursorPtr);

		//! Destroys all registered cursors.
		/*! This is performed automatically in the CursorManager destructor */
		void destroyAllCursors();
	protected:
		void cursorRegisterDraw(Cursor* cursorPtr);
		void cursorUnregisterDraw(Cursor* cursorPtr);

		//!\internal is this cursor registered with CursorManager
		bool isCursorRegistered(Cursor* cursorPtr);

		//!\internal is this cursor registered to draw
		bool isCursorDrawRegistered(Cursor* cursorPtr);

		//!\internal returns the render op list generated by the cursors that are registered for drawing
		Render::RenderOperationList getCursorRenderOpList();

		/*!\internal \brief Sets the current cursor by name.
			If the cursor name given is not found, then the CursorManager
			will attempt to find it every frame until either setCursor is called
			again with a new cursor name, or the cursor name resolves to a
			cursor (probably because it was just added).
		*/
		void setCursor(std::string cursorName);
	
		//performs the actual cursor change, complete with the necessary notifications
		void _changeCursor(Cursor* newCursor);

		//This function is called every frame. If the current cursor is not resolved, then it tries again, otherwise it does nothing.
		void _updateCursor();
	private:
		
		typedef std::map<std::string,Cursor*> CursorPtrMap;
		CursorPtrMap mCursorMap;

		typedef std::list<Cursor*> CursorPtrList;
		CursorPtrList mCursorDrawList;

		std::string mCurrentCursorStr;
		Cursor* mCurrentCursorPtr;
		bool mCurrentCursorInvalid;
	};
};//namespace OpenGUI{
#endif

