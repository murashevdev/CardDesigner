//---------------------------------------------------------------------------

#ifndef MainFormUnitH
#define MainFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <Buttons.hpp>
#include "ImageKit.h"
#include "LabelKit.h"
#include <ComCtrls.hpp>
#include <xmldoc.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel2;
   TMainMenu *MainMenu1;
   TMenuItem *N1;
   TMenuItem *N2;
   TMenuItem *MI_Exit;
   TMenuItem *MI_CreatePrj;
   TScrollBox *WorkArea;
   TImage *Image1;
   TSpeedButton *SB_NewLabel;
   TSpeedButton *SB_NewImage;
   TSpeedButton *SB_Select;
   TStringGrid *SG_ObjectInspector;
   TComboBox *CB_SelectObjectList;
   TStatusBar *StatusBar1;
   TOpenDialog *OpenPictureDlg;
   TMenuItem *MI_SavePrj;
   TMenuItem *MI_OpenPrj;
   TSaveDialog *SaveProjectDlg;
   TOpenDialog *OpenProjectDlg;
   TMenuItem *MI_Print;
   TMenuItem *N3;
   TPrintDialog *PrintDialog1;
   TMenuItem *MI_SaveImage;
   TSaveDialog *SaveImageDlg;
   TMenuItem *N4;
   TMenuItem *N5;
   TBevel *Bevel1;
   TPopupMenu *PopupMenu1;
   TMenuItem *MI_Delete;
   void __fastcall MI_ExitClick(TObject *Sender);
   void __fastcall MI_CreatePrjClick(TObject *Sender);
   void __fastcall SG_ObjectInspectorSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
   void __fastcall SG_ObjectInspectorKeyPress(TObject *Sender, char &Key);
   void __fastcall LabelKit1Select(TObject *Sender);
   void __fastcall LabelKit1Change(TObject *Sender);
   //Функции для работы с отображением компонентов в инспекторе
   void __fastcall FontSelectClick(TObject *Sender);
   void __fastcall FileSelectClick(TObject *Sender);
   void __fastcall ListSelectClick(TObject *Sender);
   void __fastcall SelectToolClick(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall WorkAreaMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
   void __fastcall CB_SelectObjectListChange(TObject *Sender);
   void __fastcall MI_SavePrjClick(TObject *Sender);
   void __fastcall MI_OpenPrjClick(TObject *Sender);
   void __fastcall MI_PrintClick(TObject *Sender);
   void __fastcall MI_SaveImageClick(TObject *Sender);
   void __fastcall MI_DeleteClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   AnsiString SurfaceFileName;
   TSpeedButton *SelectTool;
   //Хранилище для выделенного объекта
   TCustomKit *SelectObject;
   //Компоненты для инспектора объектов
   TSpeedButton *ParamButton;
   TComboBox *ParamComboBox;
   TFontDialog *FontDialog1;
   void __fastcall ClearViewingComp();
   void __fastcall LoadParamToObjInspector(TCustomKit *AParamObj);
   void __fastcall RefreshObjectsList();
   void __fastcall LoadSurface(AnsiString AFileName);
   void __fastcall CreateCompObject(_di_IXMLNode CurrentObject);
   void __fastcall ResyncWorkArea();
   void __fastcall CreateCompImage(Graphics::TBitmap *Aimage,bool DrawFon);
   void __fastcall FlashProject();
   __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
