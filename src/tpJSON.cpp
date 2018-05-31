/******************************************************************************
 * $Id: ocpn_draw_pi.h,v 1.0 2015/01/28 01:54:37 jongough Exp $
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN General Drawing Plugin
 * Author:   Jon Gough
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
 *   $EMAIL$                                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */
//#define _2_9_x_ // uncomment this to compile for 2.9.x

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#include <wx/jsonreader.h>
#include "wx/jsonwriter.h"

#include "testplugin_pi.h"
#include "tpJSON.h"
#include "ODAPI.h"
#include "version.h"

#include <stdio.h>

extern testplugin_pi        *g_testplugin_pi;
extern ODAPI                *g_pODAPI;
extern double               g_dVar;
extern wxString             g_ReceivedODAPIMessage;
extern wxJSONValue          g_ReceivedODAPIJSONMsg;


tpJSON::tpJSON()
{
    // ctor
}

tpJSON::~tpJSON()
{
    // dtor
}

void tpJSON::ProcessMessage(wxString &message_id, wxString &message_body)
{
    wxJSONValue     root;
    wxJSONReader    reader;
    wxJSONValue     jMsg;
    wxJSONWriter    writer;
    wxString        MsgString;
    
    wxString    sLogMessage;
    wxString    l_sType;
    wxString    l_sMsg;
    double      l_dLat;
    double      l_dLon;
    double      l_dStartLat;
    double      l_dStartLon;
    double      l_dEndLat;
    double      l_dEndLon;
    double      l_dCrossingLat;
    double      l_dCrossingLon;
    double      l_dCrossingDist;
    wxString    l_GUID;
    int         l_BoundaryType;
    int         l_BoundaryState;
    bool        bFail = false;
    
    if(message_id != _T("TESTPLUGIN_PI")) {
        if(message_id == _T("OCPN_DRAW_PI_READY_FOR_REQUESTS")) {
            if(message_body == _T("TRUE")) {
                if(g_testplugin_pi->m_bReadyForRequests)
                    g_testplugin_pi->GetODAPI();
            }
        } 
    } else if(message_id == wxS("TESTPLUGIN_PI")) {
        // now read the JSON text and store it in the 'root' structure
        // check for errors before retreiving values...
        int numErrors = reader.Parse( message_body, &root );
        if ( numErrors > 0 )  {
            const wxArrayString& errors = reader.GetErrors();
            for(size_t i = 0; i < errors.GetCount(); i++)
            {
                if(i == 0) {
                    sLogMessage.clear();
                    sLogMessage.Append(wxT("testplugin_pi: Error parsing JSON message - "));
                    sLogMessage.Append( message_id );
                }
                else sLogMessage.Append(wxT(" "));
                sLogMessage.append( errors.Item( i ) );
                wxLogMessage( sLogMessage );
            }
            return;
        }
        if(!root.HasMember( wxS("Source"))) {
            // Originator
            wxLogMessage( wxS("No Source found in message") );
            bFail = true;
        }
        
        if(!root.HasMember( wxS("Msg"))) {
            // Message identifier
            wxLogMessage( wxS("No Msg found in message") );
            bFail = true;
        }
        
        if(!root.HasMember( wxS("Type"))) {
            // Message type, orig or resp
            wxLogMessage( wxS("No Type found in message") );
            bFail = true;
        }
        
        if(!root.HasMember( wxS("MsgId"))) {
            // Unique (?) Msg number
            wxLogMessage( wxS("No MsgId found in message") );
            bFail = true;
        }

        if(!bFail && root[wxS("Msg")].AsString() == wxS("Version") && root[wxS("Type")].AsString() == wxS("Request")) {
            jMsg[wxT("Source")] = wxT("TESTPLUGIN_PI");
            jMsg[wxT("Msg")] = root[wxT("Msg")];
            jMsg[wxT("Type")] = wxT("Response");
            jMsg[wxT("MsgId")] = root[wxT("MsgId")].AsString();
            jMsg[wxS("Major")] = PLUGIN_VERSION_MAJOR;
            jMsg[wxS("Minor")] = PLUGIN_VERSION_MINOR;
            jMsg[wxS("Patch")] = PLUGIN_VERSION_PATCH;
            jMsg[wxS("Date")] = PLUGIN_VERSION_DATE;
            writer.Write( jMsg, MsgString );
            SendPluginMessage( root[wxS("Source")].AsString(), MsgString );
        } else if(!bFail && root[wxS("Msg")].AsString() == wxS("Version") && root[wxS("Type")].AsString() == wxS("Response")) {
            g_ReceivedODAPIJSONMsg = root;
            g_ReceivedODAPIMessage = message_body;
        } else if(root[wxS("Msg")].AsString() == wxS("GetAPIAddresses") ) {
            g_ReceivedODAPIJSONMsg = root;
            g_ReceivedODAPIMessage = message_body;
            
        } else if(!bFail && root[wxS("Msg")].AsString() == wxS("CreateBoundary")) {
            if(!root.HasMember( wxS("BoundaryName"))) {
                wxLogMessage( wxS("No BoundaryName found in message") );
                bFail = true;
            }
            
            if(!root.HasMember( wxS("BoundaryType"))) {
                wxLogMessage( wxS("No BoundaryType type found in message") );
                bFail = true;
            }
            
            if(!root.HasMember( wxS("BoundaryPoints"))) {
                wxLogMessage( wxS("No BoundaryPoints type found in message") );
                bFail = true;
            }
            
            if(!bFail) {
                if(root[wxS("Type")].AsString() == _T("Response")) {
                    return;
                }
            }
        }
        
    } else if(message_id == _T("WMM_VARIATION_BOAT")) {
        
        // construct the JSON root object
        wxJSONValue  root;
        // construct a JSON parser
        wxJSONReader reader;
        
        // now read the JSON text and store it in the 'root' structure
        // check for errors before retreiving values...
        int numErrors = reader.Parse( message_body, &root );
        if ( numErrors > 0 )  {
            //              const wxArrayString& errors = reader.GetErrors();
            return;
        }
        
        // get the DECL value from the JSON message
        wxString decl = root[_T("Decl")].AsString();
        double decl_val;
        decl.ToDouble(&decl_val);
        
        g_dVar = decl_val;
    }
    
    
    return;
}

