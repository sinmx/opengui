// OpenGUI (http://opengui.sourceforge.net)
// This source code is release under the BSD License
// See LICENSE.TXT for details

#include "stdafx.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute( "OpenGUI.Net" )];
[assembly:AssemblyDescriptionAttribute( ".NET Framework Binding for OpenGUI" )];
[assembly:AssemblyConfigurationAttribute( "" )];
[assembly:AssemblyCompanyAttribute( "OpenGUI Project (http://opengui.sourceforge.net/)" )];
[assembly:AssemblyProductAttribute( "OpenGUI.Net" )];
[assembly:AssemblyCopyrightAttribute( "Copyright (c)  2006" )];
[assembly:AssemblyTrademarkAttribute( "" )];
[assembly:AssemblyCultureAttribute( "" )];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly:AssemblyVersionAttribute( "1.0.*" )];

[assembly:ComVisible( false )];

[assembly:CLSCompliantAttribute( true )];

[assembly:SecurityPermission( SecurityAction::RequestMinimum, UnmanagedCode = true )];