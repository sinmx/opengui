#include "OpenGUI_ContainerControl.h"

namespace OpenGUI {
	//############################################################################
	class ContainerControl_ObjectAccessorList : public ObjectAccessorList {
	public:
		ContainerControl_ObjectAccessorList() {
			/* Currently has no accessors to bind */
		}
		~ContainerControl_ObjectAccessorList() {}
	}
	gContainerControl_ObjectAccessorList;

	//############################################################################
	//############################################################################


	//############################################################################
	ContainerControl::ContainerControl() {
		if ( gContainerControl_ObjectAccessorList.getParent() == 0 )
			gContainerControl_ObjectAccessorList.setParent( Control::getAccessors() );

		// initialize state variables
		m_LayoutSuspended = false; // layouts are instantaneous by default
		m_LayoutValid = true; // layout begins valid (as there are no controls to update, it does not matter)
		m_InUpdateLayout = false; // we are not in updateLayout() quite yet

		// set up defaults for properties

		//Set up events and default bindings
		getEvents().createEvent( "DrawBG" );
		getEvents()["DrawBG"].add( new EventDelegate( this, &ContainerControl::onDrawBG ) );
		getEvents().createEvent( "ChildAttached" );
		getEvents().createEvent( "ChildDetached" );
		getEvents()["ChildAttached"].add( new EventDelegate( this, &ContainerControl::onChildAttached ) );
		getEvents()["ChildDetached"].add( new EventDelegate( this, &ContainerControl::onChildDetached ) );
	}
	//############################################################################
	ContainerControl::~ContainerControl() {
		/**/
	}
	//############################################################################
	ObjectAccessorList* ContainerControl::getAccessors() {
		return &gContainerControl_ObjectAccessorList;
	}
	//############################################################################
	char* ContainerControl::getClassName() {
		return "OpenGUI::ContainerControl";
	}
	//############################################################################
	void ContainerControl::eventDrawBG( Brush& brush ) {
		Draw_EventArgs event( brush );
		triggerEvent( "DrawBG", event );
	}
	//############################################################################
	void ContainerControl::onDrawBG( Object* sender, Draw_EventArgs& evtArgs ) {
		/* Default is to do nothing */
	}
	//############################################################################
	void ContainerControl::onChildAttached( Object* sender, Attach_EventArgs& evtArgs ) {
		invalidate(); // need to invalidate caches for hierarchy change
	}
	//############################################################################
	void ContainerControl::onChildDetached( Object* sender, Attach_EventArgs& evtArgs ) {
		invalidate(); // need to invalidate caches for hierarchy change
	}
	//############################################################################
	void ContainerControl::eventChildAttached( I_WidgetContainer* container, Widget* newChild ) {
		Attach_EventArgs event( container, newChild );
		triggerEvent( "ChildAttached", event );
	}
	//############################################################################
	void ContainerControl::eventChildDetached( I_WidgetContainer* container, Widget* prevChild ) {
		Attach_EventArgs event( container, prevChild );
		triggerEvent( "ChildDetached", event );
	}
	//############################################################################
	/*! The \c Cursor_Move event is only re-issued to children if the cursor is 
	currently inside the container, or if the cursor just left the container. */
	void ContainerControl::onCursor_Move( Object* sender, Cursor_EventArgs& evtArgs ){
		Control::onCursor_Move(sender,evtArgs);
		static bool lastInside = false;
		bool reissue = lastInside;
		if ( _isInside( evtArgs.Position ) )
			lastInside = reissue = true;
		else
			lastInside = false;

		if(reissue){
			FVector2 newPos;
			newPos = evtArgs.Position;
			newPos.x += m_ClientAreaOffset_UL.x;
			newPos.y += m_ClientAreaOffset_UL.y;
			for ( WidgetCollection::reverse_iterator iter = Children.rbegin();
				iter != Children.rend(); iter++ ) {
					iter->eventCursor_Move(newPos.x, newPos.y);
			}
		}
	}
	//############################################################################
	void ContainerControl::_draw( Brush& brush ) {
		if ( getVisible() ) {
			brush.pushAlpha( getAlpha() );

			//draw background
			brush._pushMarker( this );
			eventDrawBG( brush );
			brush._popMarker( this );

			//draw children
			brush.pushPosition( getPosition() );
			for ( WidgetCollection::reverse_iterator iter = Children.rbegin();
					iter != Children.rend(); iter++ ) {
				iter->_draw( brush );
			}
			brush.pop(); // pop client area position offset

			//draw foreground
			brush._pushMarker( this );
			eventDraw( brush );
			brush._popMarker( this );

			brush.pop(); // pop alpha
		}
	}
	//############################################################################
	/*! If layout is not suspended, this triggers an immediate update of the layout.
	This function is protected from re-entry when a layout operation is currently
	under way. */
	void ContainerControl::invalidateLayout() {
		if ( !m_InUpdateLayout ) {
			m_LayoutValid = false;
			if ( !m_LayoutSuspended )
				updateLayout();
		}
	}
	//############################################################################
	void ContainerControl::suspendLayout() {
		m_LayoutSuspended = true;
	}
	//############################################################################
	void ContainerControl::resumeLayout() {
		m_LayoutSuspended = false;
		if ( !layoutValid() )
			updateLayout();
	}
	//############################################################################
	bool ContainerControl::layoutValid() {
		return m_LayoutValid;
	}
	//############################################################################
	/*! This is a convenience function that calculates the client area into a rect
	from the defined offsets \c m_ClientAreaOffset_UL and \c m_ClientAreaOffset_LR.
	*/
	const FRect& ContainerControl::getClientArea() {
		static FRect retval;
		FVector2 size = getSize();
		retval.min.x = m_ClientAreaOffset_UL.x;
		retval.min.y = m_ClientAreaOffset_UL.y;
		retval.max.x = size.x + m_ClientAreaOffset_LR.x;
		retval.max.y = size.y + m_ClientAreaOffset_LR.y;
		return retval;
	}
	//############################################################################
	/*! The update is performed regardless of layout being currently suspended,
	so only call this if you really want the update to occur. Layout validity,
	however, is still honored and layouts will not occur unless needed. */
	void ContainerControl::updateLayout() {
		m_InUpdateLayout = true;
		if ( !layoutValid() ) {
			_setChildControlLayoutState( true );
			_doUpdateLayout();
			m_LayoutValid = true;
			_setChildControlLayoutState( false );
		}
		m_InUpdateLayout = false;
	}
	//############################################################################
	void ContainerControl::_setChildControlLayoutState( bool state ) {
		for ( WidgetCollection::iterator iter = Children.begin();
				iter != Children.end(); iter++ ) {
			Control* c = dynamic_cast<Control*>( iter.get() );
			if ( c )
				c->m_InLayout = state;
		}
	}
	//############################################################################
	/*! This is virtual in the case that a subclass wishes to perform their own layout logic.
	Toolbars and other specialty containers would likely need this functionality. */
	void ContainerControl::_doUpdateLayout() {
		static FRect oldClntArea = getClientArea();
		FRect clntArea = getClientArea();

		//update all docked controls
		for ( WidgetCollection::iterator iter = Children.begin();
				iter != Children.end(); iter++ ) {
			Control* ctrl = dynamic_cast<Control*>( iter.get() );
			if ( ctrl ) {
				int dock = ctrl->getDocking();
				if ( dock ) {

					//Next two IF's cover position and height for 'Fill' as well
					if (( dock & Control::Left ) || ( dock & Control::Right ) ) {
						ctrl->setTop( clntArea.getPosition().y );
						ctrl->setHeight( clntArea.getHeight() );
					}
					if (( dock & Control::Top ) || ( dock & Control::Bottom ) ) {
						ctrl->setLeft( clntArea.getPosition().x );
						ctrl->setWidth( clntArea.getWidth() );
					}

					if ( dock == Control::Left ) {
						ctrl->setLeft( clntArea.getPosition().x );
						clntArea.setWidth( clntArea.getWidth() - ctrl->getWidth() );
						clntArea.offset( FVector2( ctrl->getWidth(), 0.0f ) );
					}
					if ( dock == Control::Right ) {
						ctrl->setLeft( clntArea.getPosition().x + clntArea.getWidth() - ctrl->getWidth() );
						clntArea.setWidth( clntArea.getWidth() - ctrl->getWidth() );
					}

					if ( dock == Control::Top ) {
						ctrl->setTop( clntArea.getPosition().y );
						clntArea.setHeight( clntArea.getHeight() - ctrl->getHeight() );
						clntArea.offset( FVector2( 0.0f, ctrl->getHeight() ) );
					}
					if ( dock == Control::Bottom ) {
						ctrl->setTop( clntArea.getPosition().y + clntArea.getHeight() - ctrl->getHeight() );
						clntArea.setHeight( clntArea.getHeight() - ctrl->getHeight() );
					}
				}
			}
		}

		//use difference of new and previous client area space to update anchored controls
		float deltaLeft = clntArea.getPosition().x - oldClntArea.getPosition().x;
		float deltaTop = clntArea.getPosition().y - oldClntArea.getPosition().y ;
		float deltaRight = ( clntArea.getPosition().x + clntArea.getWidth() ) - ( oldClntArea.getPosition().x + oldClntArea.getWidth() ) ;
		float deltaBottom = ( clntArea.getPosition().y + clntArea.getHeight() ) - ( oldClntArea.getPosition().y + oldClntArea.getHeight() ) ;

		for ( WidgetCollection::iterator iter = Children.begin();
				iter != Children.end(); iter++ ) {
			Control* ctrl = dynamic_cast<Control*>( iter.get() );
			if ( ctrl ) {
				int dock = ctrl->getDocking();
				if ( dock == Control::None ) {
					int anchor = ctrl->getAnchor();
					if ( anchor & Control::Left ) {
						ctrl->setLeft( ctrl->getLeft() + deltaLeft );
						if ( anchor & Control::Right )
							ctrl->setWidth(( ctrl->getWidth() - deltaLeft ) + deltaRight );
					}
					if (( anchor & Control::Left ) == 0 && ( anchor & Control::Right ) ) {
						ctrl->setLeft( ctrl->getLeft() + deltaRight );
					}

					if ( anchor & Control::Top ) {
						ctrl->setTop( ctrl->getTop() + deltaTop );
						if ( anchor & Control::Bottom )
							ctrl->setHeight(( ctrl->getHeight() - deltaTop ) + deltaBottom );
					}
					if (( anchor & Control::Top ) == 0 && ( anchor & Control::Bottom ) ) {
						ctrl->setTop( ctrl->getTop() + deltaBottom );
					}
				}
			}
		}
		oldClntArea = getClientArea(); // update oldClntArea for next pass
	}
	//############################################################################
} // namespace OpenGUI {
