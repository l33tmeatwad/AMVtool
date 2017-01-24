// ZenLib::Server::Html::Request - A HTML request
// Copyright (C) 2008-2010 MediaArea.net SARL, Info@MediaArea.net
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include "ZenLib/Conf_Internal.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include "ZenLib/Format/Html/Html_Request.h"
//---------------------------------------------------------------------------

namespace ZenLib
{

namespace Format
{

namespace Html
{

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Request::Request()
{
    //Config
    Html=new ZenLib::Format::Html::Handler;
    IsCopy=false;
}

//---------------------------------------------------------------------------
Request::Request(const Request &Req)
{
    //Config
    Html=Req.Html;
    IsCopy=true;
}

//---------------------------------------------------------------------------
Request::~Request()
{
    //Config
    if (!IsCopy)
        delete Html; //Html=NULL
}

} //Namespace

} //Namespace

} //Namespace
