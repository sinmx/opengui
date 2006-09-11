#include "OpenGUI.h"


namespace OpenGUI {
	namespace Widgets {

		//#####################################################################
		Widget::Widget() {
			PropertySet_BindProperty( "Position", OpenGUI::PT_FVECTOR2, &Widget::_prop_SetPos, &Widget::_prop_GetPos );
			PropertySet_BindProperty( "Size", OpenGUI::PT_FVECTOR2, &Widget::_prop_SetSize, &Widget::_prop_GetSize );
			PropertySet_BindProperty( "Enabled", OpenGUI::PT_BOOL, &Widget::_prop_SetEnabled, &Widget::_prop_GetEnabled );
			PropertySet_BindProperty( "Alpha", OpenGUI::PT_FLOAT, &Widget::_prop_SetAlpha, &Widget::_prop_GetAlpha );
		}
		//#####################################################################
		Render::RenderOperationList Widget::buildWidgetRenderOpList() {
			Render::PrimitiveBox box;
			box.setRect( this->getRect() );
			return box.getRenderOperationList();
		}
		//#####################################################################
		bool Widget::_prop_SetAlpha( PropertySet* widget, const std::string& propertyName, const std::string& newValueStr, const void* newValuePtr ) {
			Widget* w = static_cast<Widget*>( widget );
			if ( !newValuePtr ) return false;
			float newAlpha = *static_cast<const float*>( newValuePtr );
			w->setAlpha( newAlpha );
			return true;
		}
		//#####################################################################
		bool Widget::_prop_GetAlpha( PropertySet* widget, const std::string& propertyName, std::string& curValue ) {
			Widget* w = static_cast<Widget*>( widget );
			PropertyParser::toStrFloat( w->getAlpha(), curValue );
			return true;
		}
		//#####################################################################
		bool Widget::_prop_SetPos( PropertySet* widget, const std::string& propertyName, const std::string& newValueStr, const void* newValuePtr ) {
			Widget* w = static_cast<Widget*>( widget );
			if ( !newValuePtr ) return false;
			FVector2 newPos = *static_cast<const FVector2*>( newValuePtr );
			w->setPos( newPos );
			return true;
		}
		//#####################################################################
		bool Widget::_prop_GetPos( PropertySet* widget, const std::string& propertyName, std::string& curValue ) {
			Widget* w = static_cast<Widget*>( widget );
			PropertyParser::toStrFVector2( w->getRect().getPosition(), curValue );
			return true;
		}
		//#####################################################################
		bool Widget::_prop_SetSize( PropertySet* widget, const std::string& propertyName, const std::string& newValueStr, const void* newValuePtr ) {
			Widget* w = static_cast<Widget*>( widget );
			if ( !newValuePtr ) return false;
			FVector2 newSize = *static_cast<const FVector2*>( newValuePtr );
			w->setSize( newSize );
			return true;
		}
		//#####################################################################
		bool Widget::_prop_GetSize( PropertySet* widget, const std::string& propertyName, std::string& curValue ) {
			Widget* w = static_cast<Widget*>( widget );
			PropertyParser::toStrFVector2( w->getRect().getSize(), curValue );
			return true;
		}
		//#####################################################################
		bool Widget::_prop_SetEnabled( PropertySet* widget, const std::string& propertyName, const std::string& newValueStr, const void* newValuePtr ) {
			Widget* w = static_cast<Widget*>( widget );
			if ( !newValuePtr ) return false;
			bool newEnabled = *static_cast<const bool*>( newValuePtr );
			w->setDisabled( !newEnabled );
			return true;
		}
		//#####################################################################
		bool Widget::_prop_GetEnabled( PropertySet* widget, const std::string& propertyName, std::string& curValue ) {
			Widget* w = static_cast<Widget*>( widget );
			PropertyParser::toStrBool( w->isEnabled(), curValue );
			return true;
		}
		//#####################################################################
	};
};


