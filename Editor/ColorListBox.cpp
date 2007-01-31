// ColorListBox.cpp : implementation file
//

#include "stdafx.h"
//#include "ami.h"
#include "ColorListBox.h"
#include "Colors.h"


/////////////////////////////////////////////////////////////////////////////
// CColorListBox

CColorListBox::CColorListBox()
{
   m_bInitialized = false;
}

CColorListBox::~CColorListBox()
{
	m_arsText.RemoveAll();

}


BEGIN_MESSAGE_MAP(CColorListBox, CListBox)
	//{{AFX_MSG_MAP(CColorListBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorListBox message handlers

void CColorListBox::DrawItem(LPDRAWITEMSTRUCT pDIStruct) 
{
   CString  sColor = "";                       // No Need To Reallocate Each Time

   HDC		   hdc = pDIStruct->hDC;
   CRect	   rItemRect( pDIStruct -> rcItem );
   CRect	   rBlockRect( rItemRect );
   CRect       rTextRect( rBlockRect );
   CBrush      brFrameBrush;
   int         iFourthWidth = 0;
   int         iItem = pDIStruct -> itemID;
   int         iAction = pDIStruct -> itemAction;
   int         iState = pDIStruct -> itemState;
   COLORREF    crColor = 0;
   COLORREF    crNormal = GetSysColor( COLOR_WINDOW );
   COLORREF    crSelected = GetSysColor( COLOR_HIGHLIGHT );
   COLORREF    crText = GetSysColor( COLOR_WINDOWTEXT );
   
   iFourthWidth = ( rBlockRect.Width() / 4 );            // Get 1/4 Of Item Area
   brFrameBrush.CreateStockObject( BLACK_BRUSH );        // Create Black Brush

   if( iState & ODS_SELECTED )                        // If Selected
   {  	  
	  SetTextColor(hdc, ( 0x00FFFFFF & ~( crText ) ) ); // Set Selected Attributes
	  SetBkColor(hdc, crSelected );             // Set BG To Highlight Color
	  ::SetBkColor(hdc, crSelected);
	  ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rBlockRect, NULL, 0, NULL);
   }
   else                                      // If Not Selected
   {                                         // Set Standard Attributes
      SetTextColor(hdc, crText );               // Set Text Color
      SetBkColor(hdc, crNormal );               // Set BG Color
      ::SetBkColor(hdc, crNormal);
	  ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rBlockRect, NULL, 0, NULL);	  
   }
   if( iState & ODS_FOCUS )                       // If Item Has The Focus
      
	  DrawFocusRect(hdc, &rItemRect );      // Draw Focus Rect

   //
   // Calculate Text Area
   //
   rTextRect.left += ( iFourthWidth + 2 );        // Set Start Of Text
   rTextRect.top += 2;                            // Offset A Bit

   //
   // Calculate Color Block Area
   //
   rBlockRect.DeflateRect( CSize( 2, 2 ) );        // Reduce Color Block Size
   rBlockRect.right = iFourthWidth;                // Set Width Of Color Block

   //
   // Draw Color Text And Block
   //
   if( iItem != -1 )                            // If Not An Empty Item
   {
      int n = GetTextLen(iItem);      
	  GetText( iItem, sColor.GetBuffer(n));                     // Get Color Text
	  sColor.ReleaseBuffer();
	  sColor = m_arsText[iItem];
      if( iState & ODS_DISABLED )                     // If Disabled
      {
         crColor = GetSysColor( COLOR_INACTIVECAPTIONTEXT );
         SetTextColor(hdc, crColor );           // Set Text Color
      }
      else                                   // If Normal
         crColor = GetItemData( iItem );              // Get Color Value

      SetBkMode(hdc, TRANSPARENT );             // Transparent Background
      TextOut(hdc, rTextRect.left, rTextRect.top,
            sColor, sColor.GetLength() );                           // Draw Color Name
	  
      ::SetBkColor(hdc, crColor);
      ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rBlockRect, NULL, 0, NULL);
      ::FrameRect(hdc, &rBlockRect, (HBRUSH)brFrameBrush.GetSafeHandle());
	  
	  
   }
   //dcContext.Detach();                             // Detach DC From Object
   // then releasing the DC itself
   //::ReleaseDC(m_hWnd, hdc);
}

void CColorListBox::PreSubclassWindow() 
{	
   Initialize();                                // Initialize Contents
   
   CListBox::PreSubclassWindow();               // Subclass Control

   SetCurSel( 0 );                              // Select First Item By Default

   return;                                      // Done!
}

int CColorListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
   Initialize();                                // Initialize Contents
   SetCurSel( 0 );                              // Select First Item By Default
	
	return 0;
}

void CColorListBox::Initialize( void )
{
   int      iAddedItem = -1;

   if( m_bInitialized )                         // If Already Initialized
      return;                                   // Return

   ResetContent();
   m_arsText.SetSize(0,1);

   m_bInitialized = true;                          // Set Initialized Flag
}

COLORREF CColorListBox::GetSelectedColorValue( void )
{
   int      iSelectedItem = GetCurSel();              // Get Selected Item

   if( iSelectedItem == CB_ERR )                   // If Nothing Selected
      return( RGB( 0, 0, 0 ) );                    // Return Black

   return( GetItemData( iSelectedItem ) );               // Return Selected Color
}


CString CColorListBox::GetSelectedColorName( void )
{
   int      iSelectedItem = GetCurSel();           // Get Selected Item

   if( iSelectedItem == CB_ERR )                   // If Nothing Selected
      return( m_sColorName = CString() );     // Return Nothing (Not "Black!")

   GetText( iSelectedItem, m_sColorName );          // Store Name Of Color

   return( m_sColorName );                         // Return Selected Color Name
}


void CColorListBox::SetSelectedColorValue( COLORREF crClr )
{
   int      iItems = GetCount();
   
   for( int iItem = 0; iItem < iItems; iItem++ )
   {
      if( crClr == GetItemData( iItem ) )         // If Match Found
         SetCurSel( iItem );                      // Select It
   }
   return;                                      // Done!
}


void CColorListBox::SetSelectedColorName( LPCTSTR cpColor )
{
   int      iItems = GetCount();
   CString  sColorName;

   for( int iItem = 0; iItem < iItems; iItem++ )
   {
      GetText( iItem, sColorName );                 // Get Color Name

      if( !sColorName.CompareNoCase( cpColor ) )   // If Match Found
         SetCurSel( iItem );                       // Select It
   }
   return;                                      // Done!
}


bool CColorListBox::RemoveColor( LPCTSTR cpColor )
{
   int      iItems = GetCount();
   CString  sColor;
   bool  bRemoved = false;

   for( int iItem = 0; iItem < iItems; iItem++ )
   {
      GetText( iItem, sColor );                     // Get Color Name
      if( !sColor.CompareNoCase( cpColor ) )          // If Match Found
      {
         DeleteString( iItem );                    // Remove It
         bRemoved = true;                       // Set Flag
         break;                                 // Stop Checking
      }
   }
   return( bRemoved );                             // Done!
}


bool CColorListBox::RemoveColor( COLORREF crClr )
{
   int      iItems = GetCount();
   bool  bRemoved = false;

   for( int iItem = 0; iItem < iItems; iItem++ )
   {
      if( crClr == GetItemData( iItem ) )       // If Desired Color Found
      {
         DeleteString( iItem );                 // Remove It
         bRemoved = true;                       // Set Flag
         break;                                 // Stop Checking
      }
   }
   return( bRemoved );                             // Done!
}


int CColorListBox::AddColor( CString cpName, COLORREF crColor )
{
   int      iItem = -1;

   iItem = AddString(cpName );             // Insert String
   m_arsText.Add(cpName);
   if( iItem != LB_ERR )                           // If Insert Good
      SetItemData( iItem, crColor );               // Set Color Value 

   return( iItem );                             // Done! Return Location
}


   

void CColorListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	
}
