
/*� copyright 1991-96 UserLand Software, Inc. All Rights Reserved.*/


/*
Briefly -- this is a very small program that demonstrates the use of the 
Menu Sharing Toolkit. For detailed info, see the enclosed readme file.

You can search the source code for menu sharing highlights by searching 
for the following string: "See Step #". The step numbers refer to steps 
in the "Cookbook" section of the enclosed readme.

We hope you like menu sharing and add it to your Macintosh application.

Thanks!
*/

#if !defined (THINK_C) || !defined (__MWERKS__)
	
	#include <Menus.h>
	#include <Windows.h>
	#include <Events.h>
	#include <Fonts.h>
	
#endif

#include <menusharing.h> /*See Step #5*/


#define applemenu 128 /*the resource id of the apple menu*/

#define aboutitem 1 /*the about command*/

#define filemenu 129 /*resource id of file menu, shared menus appear to right of this*/

#define firstsharedmenu filemenu+1 /*resource id for first shared menu*/

#define quititem 1 /*the single item in the file menu -- this is a very simple program!*/

MenuHandle happlemenu, hfilemenu; /*the two fixed, non-shared menus in this program*/

WindowPtr mainwindow = nil; /*the menu sharing test window*/

Str255 windowmessage; /*the message that's displayed in the main window*/

Boolean flexitmainloop = false; /*when true we fall thru the main event loop*/

Boolean flcurrentlyactive = true;




static void copystring (Str255 source, Str255 dest) {

	/*
	create a copy of source in dest.  copy the length byte and
	all the characters in the source string.

	assume the strings are pascal strings, with the length byte in
	the first character of the string.
	*/

	short i, len;
	
	len = (short) source [0];
	
	for (i = 0; i <= len; i++) 
		dest [i] = source [i];
	} /*copystring*/


static void ellipsize (Str255 s, short width) {

	/*
	if the string fits inside the given number of pixels, fine -- do nothing
	and return.
	
	if not, return a string that does fit, with ellipses representing the 
	deleted characters.  ellipses are generated by pressing option-semicolon.
	*/
	
	char len;
	short newwidth;
	
	if ((newwidth = StringWidth (s)) <= width) /*nothing to do, the string fits*/
		return;
	
	len = s [0]; /* current length in characters*/
	
	width -= CharWidth ('�'); /* subtract width of ellipses*/
		
	do { /*until it fits (or we run out of characters)*/
	
		newwidth -= CharWidth (s [len]);
		
		--len;
	} while ((newwidth > width) && (len != 0));
	
	++len; /*make room for the ellipses*/
	
	s [len] = '�'; 
	
	s [0] = (char) len;
	} /*ellipsize*/


static void centerstring (Rect r, Str255 s) {
	
	/*
	draw the string in the current font, size and style, centered inside
	the indicated rectangle.
	*/
	
	short rh = r.bottom - r.top;
	short rw = r.right - r.left;
	short h, v;
	FontInfo fi;
	
	GetFontInfo (&fi);
	
	ellipsize (s, rw); /*make sure it fits inside the rectangle, width-wise*/
	
	h = r.left + ((rw - StringWidth (s)) / 2);
	
	v = r.top + ((rh - (fi.ascent + fi.descent)) / 2) + fi.ascent;
	
	MoveTo (h, v);
	
	ClipRect (&r);
	
	DrawString (s);
	} /*centerstring*/


static void setfontsizestyle (short fontnum, short fontsize, short fontstyle) {

	TextFont (fontnum);
	
	TextSize (fontsize);
	
	TextFace (fontstyle);
	} /*setfontsizestyle*/
	
	
static void updatemainwindow (void) {
	
	Rect r;
	Str255 s;
	
	r = (*mainwindow).portRect;
	
	EraseRect (&r);
	
	setfontsizestyle (helvetica, 12, 0);
	
	centerstring (r, windowmessage);
	
	NumToString (FreeMem () / 1024, s);
	
	MoveTo (r.left + 3, r.bottom - 3);
	
	setfontsizestyle (geneva, 9, 0);
	
	DrawString (s);
	
	DrawString ("\pK");
	} /*updatemainwindow*/
	
	
static Boolean setwindowmessage (Str255 s) {
	
	copystring (s, windowmessage);
	
	SetPort (mainwindow);
	
	updatemainwindow ();
	
	return (true);
	} /*setwindowmessage*/


static Boolean initmainwindow (void) {
	
	mainwindow = GetNewWindow (128, nil, (WindowPtr) -1);
	
	if (mainwindow == nil)
		return (false);
	
	ShowWindow (mainwindow);
	
	return (true);
	} /*initmainwindow*/


static void handleupdate (EventRecord *ev) {
	
	WindowPtr w;
	
	w = (WindowPtr) (*ev).message;
	
	BeginUpdate (w);
	
	SetPort (w);
	
	updatemainwindow ();
	
	EndUpdate (w);
	} /*handleupdate*/


static void handledrag (EventRecord *ev, WindowPtr w) {
	
	Rect r;

	r = qd.screenBits.bounds; 
	
	r.top = r.top + GetMBarHeight (); 
	
	InsetRect (&r, 4, 4);
	
	DragWindow (w, (*ev).where, &r);
	} /*handledrag*/


static void handlemenu (long codeword) {
	
	short idmenu, iditem;
	
	iditem = LoWord (codeword);
	
	idmenu = HiWord (codeword);
	
	if (SharedMenuHit (idmenu, iditem)) /*See Step #3*/	
		goto exit;
	
	switch (idmenu) {
	
		case applemenu: 
			switch (iditem) {
				
				case aboutitem:
					Alert (262, nil);
					
					break;
			
				default: {
					Str255 s;
					
					GetItem (happlemenu, iditem, s);
					
					OpenDeskAcc (s);
					
					break;
					}
				} /*switch*/
			
			break; /*apple menu*/

		case filemenu: 
			switch (iditem) {
				
				case quititem:
				
					flexitmainloop = true;
					
					break;
				} /*switch*/
			
			break; /*file menu*/
			
		} /*switching on which menu was invoked*/
		
	exit:
	
	HiliteMenu (0);
	} /*handlemenu*/


static void handlemouse (EventRecord *ev) {

	short part;
	WindowPtr w;
	
	part = FindWindow ((*ev).where, &w);
	
	if (w != nil) 
	
		if (w != FrontWindow ()) { /*just like all other Mac programs*/
			
			SelectWindow (w);
							
			return; /*the mouse click is consumed by the bringtofront operation*/
			}
	
	switch (part) {
	
		case inMenuBar: 
			handlemenu (MenuSelect ((*ev).where)); 
			
			break;
		
		case inSysWindow:
			SystemClick (ev, w); 
			
			break;
		
		case inDrag:
			handledrag (ev, w);
			
			break;
		
		case inContent:
			EnableSharedMenus (false);
			
			while (StillDown ())
				;
			
			EnableSharedMenus (true);
			
			break;
			
		} /*switch*/
	} /*handlemouse*/


static void handlekeystroke (EventRecord *ev) { /*See Step #4*/

	char ch = (*ev).message & charCodeMask;
	
	if (SharedScriptRunning ()) { /*cmd-period terminates the script*/
	
		if (((*ev).modifiers & cmdKey) && (ch == '.')) { 
			
			CancelSharedScript (); /*cancel the shared menu script, if one is running*/
		
			return;
			}
		}
	
	handlemenu (MenuKey (ch)); /*not cmd-period, process the normal way*/
	} /*handlekeystroke*/


static void handlejuggle (EventRecord *ev) {
	
	flcurrentlyactive = ((*ev).message & resumeFlag) != 0;
	} /*handlejuggle*/


static void handleevent (EventRecord *ev) {
	
	switch ((*ev).what) {
	
		case keyDown: case autoKey: 
			handlekeystroke (ev);
			
			break;
			
		case mouseDown:
			handlemouse (ev);
			
			break;
		
		case updateEvt:
			handleupdate (ev);
			
			break;
		
		case osEvt:
			handlejuggle (ev);
			
			break;
		
		case kHighLevelEvent:
			AEProcessAppleEvent (ev);
			
			break;
		
		case nullEvent: /*See Step #2*/
			if (flcurrentlyactive)
				CheckSharedMenus (firstsharedmenu);
			
			break;
		} /*switch*/
	} /*handleevent*/


static void maineventloop (void) {
	
	EventRecord ev;
	
	while (!flexitmainloop) {
		
		WaitNextEvent (everyEvent, &ev, 2, nil);
		
		handleevent (&ev);
		} /*while*/
	} /*maineventloop*/


static void initmenus (void) {
	
	/*
	set up our apple and file menus.
	*/
	
	happlemenu = GetMenu (applemenu); 
	
	AddResMenu (happlemenu, 'DRVR'); 
	
	InsertMenu (happlemenu, 0); 
	
	hfilemenu = GetMenu (filemenu); 
	
	InsertMenu (hfilemenu, 0);
	
	DrawMenuBar ();
	} /*initmenus*/


static void initmacintosh (void) {

	/*
	the magic stuff that every Macintosh application needs to do 
	before doing anything else.
	*/

	short i;
		
	MaxApplZone ();
	
	for (i = 0; i < 10; i++) 
		MoreMasters ();
	
	InitGraf (&qd.thePort);
	
	InitFonts ();
	
	FlushEvents (everyEvent, 0);
	
	InitWindows ();
	
	InitMenus ();
	
	TEInit ();
	
	InitDialogs (0L);
	
	InitCursor ();
	
	for (i = 0; i < 5; i++) { /*register with Multifinder*/
		
		EventRecord ev;
		
		EventAvail (everyEvent, &ev); /*see TN180 -- splash screen*/
		} /*for*/
	} /*initmacintosh*/


static pascal OSErr handlequit (const AppleEvent *event, AppleEvent *reply, long refcon) {
	
	#pragma unused (event, reply, refcon)

	flexitmainloop = true;
	
	return (noErr);
	} /*handlequit*/
	
	
static pascal OSErr handleopenapp (const AppleEvent *event, AppleEvent *reply, long refcon) {
	
	#pragma unused (event, reply, refcon)

	return (noErr);
	} /*handleopenapp*/


static pascal OSErr handleopen (const AppleEvent *event, AppleEvent *reply, long refcon) {
	
	#pragma unused (event, reply, refcon)

	return (noErr);
	} /*handleopen*/
	
		
static pascal OSErr handleprint (const AppleEvent *event, AppleEvent *reply, long refcon) {
	
	#pragma unused (event, reply, refcon)

	return (noErr);
	} /*handleprint*/
	
		
static pascal OSErr setmessageverb (const AppleEvent *event, AppleEvent *reply, long refcon) {

	#pragma unused (refcon)

	OSErr ec;
	AEDesc result;
	Str255 s;
	Handle htext;
	long lentext;
	Boolean fl;
	
	ec = AEGetParamDesc (event, keyDirectObject, typeChar, &result);
	
	if (ec != noErr) 
		return (ec);
		
	htext = result.dataHandle;
	
	if (htext == nil)
		return (noErr);
		
	lentext = GetHandleSize (htext);
	
	if (lentext > 255)
		lentext = 255;
		
	s [0] = (unsigned char) lentext;
	
	BlockMove (*htext, &s [1], lentext);
	
	AEDisposeDesc (&result);
	
	setwindowmessage (s);
	
	fl = true;
	
	ec = AEPutParamPtr (reply, keyDirectObject, typeBoolean, (Ptr) &fl, sizeof (Boolean));
	
	return (ec);
	} /*setmessageverb*/
	

static void installhandlers (void) {

	AEInstallEventHandler ('TEST', 'smsg', NewAEEventHandlerProc (setmessageverb), 0, false);

	AEInstallEventHandler (kCoreEventClass, kAEOpenApplication, NewAEEventHandlerProc (handleopenapp), 0, false);
	
	AEInstallEventHandler (kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerProc (handleopen), 0, false);
	
	AEInstallEventHandler (kCoreEventClass, kAEPrintDocuments, NewAEEventHandlerProc (handleprint), 0, false);
	
	AEInstallEventHandler (kCoreEventClass, kAEQuitApplication, NewAEEventHandlerProc (handlequit), 0, false);
	} /*installhandlers*/
	
	
static pascal void errordialog (Str255 s) {
	
	ParamText (s, "\p", "\p", "\p"); 
	
	Alert (263, nil);
	} /*errordialog*/
	
	
static pascal void eventfilter (EventRecord *ev) {
	
	handleevent (ev); /*could receive an update, activate, OS, or null event*/
	} /*eventfilter*/


void main (void) {
	
	initmacintosh ();
	
	if (!InitSharedMenus (errordialog, eventfilter)) /*See Step #1*/
		goto error;
		
	installhandlers ();
		
	initmenus ();
	
	initmainwindow ();
	
	maineventloop ();
		
	ExitToShell ();
	
	error:
	
	Alert (261, nil); /*this application requires system 7.0 or higher*/
		
	ExitToShell ();
	} /*main*/


