//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainFormUnit.h"
#include <jpeg.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LabelKit"
#pragma link "ImageKit"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MI_ExitClick(TObject *Sender)
{//Закрыть программу
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MI_CreatePrjClick(TObject *Sender)
{//Создать новый проект
   //Открыть файл фона
   if(OpenPictureDlg->Execute() == false) return;
   FlashProject();
   LoadSurface(OpenPictureDlg->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LoadSurface(AnsiString AFileName)
{//Функция загрузки фонового изображения
   Image1->Picture->LoadFromFile(AFileName);
   //Устанвоить размер видимой области, размеру изображения
   Image1->Width  = Image1->Picture->Width;
   Image1->Height = Image1->Picture->Height;
   StatusBar1->Panels->Items[1]->Text = "Размер: "+ IntToStr(Image1->Width)+"x"+IntToStr(Image1->Height);
   //Сохранить имя файла поверхности
   SurfaceFileName = AFileName;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LoadParamToObjInspector(TCustomKit *AParamObj)
{//Загрузить значения параметров в визуальный инспектор объектов
   SG_ObjectInspector->RowCount = AParamObj->Count;
   //получить список параметров
   AParamObj->GetParams();
   for(int i=0;i < AParamObj->Count;i++)
   {
     //Отобразить имя параметра
     SG_ObjectInspector->Cells[0][i] = AParamObj->Strings[i];
     //Получить указатель на объект значения параметра
     TParamObj *CurParamVal = (TParamObj*)AParamObj->Objects[i];
     //Отобразить значение параметра по типу
     switch(CurParamVal->TypeID)
     {
     case TI_INT:
      SG_ObjectInspector->Cells[1][i] = IntToStr(CurParamVal->Value);
      break;
     case TI_STRING:
      SG_ObjectInspector->Cells[1][i] = CurParamVal->Text;
      break;
     case TI_FONT:
      SG_ObjectInspector->Cells[1][i] = CurParamVal->Font->Name;
      break;
     case TI_FILE:
      SG_ObjectInspector->Cells[1][i] = ExtractFileName(CurParamVal->Text);
      break;
     case TI_LIST:
      SG_ObjectInspector->Cells[1][i] = CurParamVal->Text;
      break;
     };
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SG_ObjectInspectorSelectCell(TObject *Sender,
      int ACol, int ARow, bool &CanSelect)
{//Событие при выборе ячейки

   //Получить указатель на объект значения параметра
   TParamObj *CurParamVal = (TParamObj*)SelectObject->Objects[ARow];
   //Очистить компоненты для работы с параметрами в испекторе
   ClearViewingComp();
   //Сохранить координаты области для размещения компонента параметров
   TRect ViewParamComp;
   ViewParamComp.Left   = SG_ObjectInspector->CellRect(ACol,ARow).Left;
   ViewParamComp.Top    = SG_ObjectInspector->CellRect(ACol,ARow).Top;
   ViewParamComp.Bottom = SG_ObjectInspector->DefaultRowHeight;
   ViewParamComp.Right  = SG_ObjectInspector->DefaultColWidth;
   //Проверить тип объекта
   switch(CurParamVal->TypeID)
   {
   case TI_FONT:
      //Отобразить кнопку для вызова дилалог работы со шрифтом
      ParamButton = new TSpeedButton(SG_ObjectInspector);
      ParamButton->Parent  = SG_ObjectInspector;
      ParamButton->Caption = "...";
      ParamButton->Left    = ViewParamComp.Left;
      ParamButton->Top     = ViewParamComp.Top;
      ParamButton->Height  = ViewParamComp.Bottom;
      ParamButton->Width   = ViewParamComp.Right;
      ParamButton->OnClick = FontSelectClick;
      //Создать диалог для выбора шрифта
      FontDialog1 = new TFontDialog(Form1);
      FontDialog1->Font->Assign(CurParamVal->Font);
      break;
   case TI_FILE:
      //Отобразить кнопку для вызова дилалог выбора файлов
      ParamButton = new TSpeedButton(SG_ObjectInspector);
      ParamButton->Parent  = SG_ObjectInspector;
      ParamButton->Caption = "...";
      ParamButton->Left    = ViewParamComp.Left;
      ParamButton->Top     = ViewParamComp.Top;
      ParamButton->Height  = ViewParamComp.Bottom;
      ParamButton->Width   = ViewParamComp.Right;
      ParamButton->OnClick = FileSelectClick;
      OpenPictureDlg->FileName = ExtractFileName(CurParamVal->Text);
      break;
   case TI_LIST:
      //Отобразить кнопку для выбора списка занчений
      ParamComboBox = new TComboBox(Form1);
      ParamComboBox->Parent  = Form1;
      ParamComboBox->Style   = csDropDownList;
      ParamComboBox->Left    = SG_ObjectInspector->Left+ViewParamComp.Left+2;
      ParamComboBox->Top     = SG_ObjectInspector->Top+ViewParamComp.Top+3;
      ParamComboBox->Height  = ViewParamComp.Bottom;
      ParamComboBox->Width   = ViewParamComp.Right;
      ParamComboBox->Items->Assign(CurParamVal->List);
      ParamComboBox->ItemIndex = ParamComboBox->Items->IndexOf(CurParamVal->Text);
      ParamComboBox->OnChange = ListSelectClick;
      break;
   };
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SG_ObjectInspectorKeyPress(TObject *Sender,
      char &Key)
{//Событие при нажатии калавиш
   if(SG_ObjectInspector->EditorMode == false) return;
   if(Key != 13) return;
   //Получить указатель на объект значения параметра
   TParamObj *CurParamVal = (TParamObj*)SelectObject->Objects[SG_ObjectInspector->Row];
   //Установить значение параметра по типу
   switch(CurParamVal->TypeID)
   {
   case TI_INT:
      CurParamVal->Value = StrToInt(SG_ObjectInspector->Cells[1][SG_ObjectInspector->Row]);
      break;
   case TI_STRING:
      //Проверка на изменение имени объекта
      CurParamVal->Text = SG_ObjectInspector->Cells[1][SG_ObjectInspector->Row];
      break;
   };
   //Вызвать функцию установки значений параметров
   SelectObject->SetParams();
   //Перезагрузить новый список занчений параметров
   LoadParamToObjInspector(SelectObject);

   //Изменить имя выделеного объекта в списке объектов
   if(SG_ObjectInspector->Cells[0][SG_ObjectInspector->Row] != "Name")
      return; //Это не поле имени объекта
   //Получить индекс выделеннго обекта из списка
   int IndexObj = CB_SelectObjectList->Items->IndexOfObject(SelectObject);
   if(IndexObj == -1) return;
   CB_SelectObjectList->Items->Strings[IndexObj] = SelectObject->CompOwner->Name;
   CB_SelectObjectList->ItemIndex = IndexObj;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FontSelectClick(TObject *Sender)
{//Событие для вызова диалога настройки шрифта
   if(FontDialog1->Execute() == false) return;
   //Получить указатель на объект значения параметра
   TParamObj *CurParamVal = (TParamObj*)SelectObject->Objects[SG_ObjectInspector->Row];
   CurParamVal->Font->Assign(FontDialog1->Font);
   SelectObject->SetParams();
   LoadParamToObjInspector(SelectObject);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FileSelectClick(TObject *Sender)
{//Событие для вызова диалога настройки шрифта
   if(OpenPictureDlg->Execute() == false) return;
   //Получить указатель на объект значения параметра
   TParamObj *CurParamVal = (TParamObj*)SelectObject->Objects[SG_ObjectInspector->Row];
   CurParamVal->Text = OpenPictureDlg->FileName;
   SelectObject->SetParams();
   LoadParamToObjInspector(SelectObject);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListSelectClick(TObject *Sender)
{//Событие для вызова диалога настройки шрифта
   //Получить указатель на объект значения параметра
   TParamObj *CurParamVal = (TParamObj*)SelectObject->Objects[SG_ObjectInspector->Row];
   CurParamVal->Text = ParamComboBox->Text;
   SelectObject->SetParams();
   LoadParamToObjInspector(SelectObject);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LabelKit1Select(TObject *Sender)
{//Событие при выборе объекта
   if(SelectObject != NULL)
   {
      //Снять режим выделения с предыдущего выделенного
      SelectObject->Selected = false;
   }
   //Отобразить параметры объекта
   SelectObject = dynamic_cast<TCustomKit*>(Sender);
   //Отобразить параметры объекта
   LoadParamToObjInspector(SelectObject);
   //Отобразить имя выделеного объекта в списке объектов
   if(CB_SelectObjectList->ItemIndex != -1)
      CB_SelectObjectList->ItemIndex = CB_SelectObjectList->Items->IndexOf(SelectObject->CompOwner->Name);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LabelKit1Change(TObject *Sender)
{//Событие при изменении состояния выделенного объекта
   //Отобразить параметры объекта
   LoadParamToObjInspector(SelectObject);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ClearViewingComp()
{//Очистить активные сомпоненты для работы с параметрами
   if(ParamButton != NULL)
   {
      delete ParamButton;
      ParamButton = NULL;
   }
   if(ParamComboBox != NULL)
   {
      delete ParamComboBox;
      ParamComboBox = NULL;
   }
   if(FontDialog1 != NULL)
   {
      delete FontDialog1;
      FontDialog1 = NULL;
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SelectToolClick(TObject *Sender)
{//Событие при выборе инструмента
   SelectTool = dynamic_cast<TSpeedButton*>(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{//Событие при отображении формы
   SelectTool = SB_Select;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::WorkAreaMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{//Событие для создания объекта на рабочей области
   //Определяем тип создаваемого объекта
   TCustomKit *ElementParam;
   AnsiString ElementName;
   switch(SelectTool->Tag)
   {
   case 0: //Не выполнять действий
      return;
   case 1: //Создать элемент "Текста"
   {
      TLabelKit *LK = new TLabelKit(WorkArea);
      //Установить обработчики событий при выделении и изменении состояния
      LK->OnChangePos = LabelKit1Change;
      LK->OnSelect    = LabelKit1Select;
      LK->Left = X-WorkArea->HorzScrollBar->Position;
      LK->Top  = Y-WorkArea->VertScrollBar->Position;
      LK->Parent = WorkArea; //Отобразить элемент на области
      //Сгенерировать имя компонента
      int LKCount = WorkArea->ComponentCount;
      do
      {
         try
         {
            LK->Name = "LabelKit"+IntToStr(LKCount);
            break;
         }
         catch(...)
         {
            ++LKCount;
         }
      }while(true);
      LK->Caption = LK->Name; //Установить текстовое значение
      LK->Tag = 0xFF; //Установить признак компонента
      LK->Params->Selected = true; //Установить состояние "выделено"
      ElementParam = LK->Params;
      ElementName  = LK->Name;
      break;
   }
   case 2: //Создать элемент "Изображение"
   {
      TImageKit *IK = new TImageKit(WorkArea);
      //Установить обработчики событий при выделении и изменении состояния
      IK->OnChangePos = LabelKit1Change;
      IK->OnSelect    = LabelKit1Select;
      IK->Left = X-WorkArea->HorzScrollBar->Position;
      IK->Top  = Y-WorkArea->VertScrollBar->Position;
      IK->Parent = WorkArea; //Отобразить элемент на области
      //Сгенерировать имя компонента
      int IKCount = WorkArea->ComponentCount;
      do
      {
         try
         {
            IK->Name = "ImageKit"+IntToStr(IKCount);
            break;
         }
         catch(...)
         {
            ++IKCount;
         }
      }while(true);
      IK->Tag = 0xFF; //Установить признак компонента
      IK->Params->Selected = true; //Установить состояние "выделено"
      ElementParam = IK->Params;
      ElementName  = IK->Name;
      break;
   }
   };
   //Добавить новый элемент в список объектов
   CB_SelectObjectList->Items->AddObject(ElementName,ElementParam);
   CB_SelectObjectList->ItemIndex = CB_SelectObjectList->Items->Count - 1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RefreshObjectsList()
{//Обновить список доступных объектов
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CB_SelectObjectListChange(TObject *Sender)
{//Событие при выборе элемента из списка
   if(CB_SelectObjectList->ItemIndex == -1) return;
   TCustomKit *CurElementParam = dynamic_cast<TCustomKit*>(CB_SelectObjectList->Items->Objects[CB_SelectObjectList->ItemIndex]);
   if(CurElementParam != NULL)
      CurElementParam->Selected = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MI_SavePrjClick(TObject *Sender)
{//Сохранить проект в формате XML
   //Вызвать диалог сохранения проекта
   if(SaveProjectDlg->Execute() == false) return;
   StatusBar1->Panels->Items[0]->Text = SaveProjectDlg->FileName;
   //Выровнить все элементы по верхнему и левому караю
   ResyncWorkArea();
   //Создать файл для сохранения проекта
   TStringList *SaveFile = new TStringList;
   TCustomKit *CurrentParamList;
   TParamObj *CurrentParam;
   //Добавить код заголовока
   SaveFile->Add("<?xml version=\"1.0\" encoding=\"Windows-1251\"?>");
   //Добавить название проекта и параметры
   AnsiString ProjectName = ExtractFileName(SaveProjectDlg->FileName);
   ProjectName = ProjectName.SubString(1,ProjectName.Length()-4);
   SaveFile->Add("<"+ProjectName+" Surface=\""+SurfaceFileName+"\">");
   //Добавить структуру сохраняемых параметров
   AnsiString FormatTagStr;
   for(int CompIndex=0;CompIndex < CB_SelectObjectList->Items->Count;CompIndex++)
   {//Пройти по всем объектам из списка доступных
      //Получить указатель на параметр
      CurrentParamList = dynamic_cast<TCustomKit*>(CB_SelectObjectList->Items->Objects[CompIndex]);
      if(CurrentParamList == NULL) continue;
      SaveFile->Add("<"+CurrentParamList->CompType+">"); //Установливаем тип объекта
      for(int ParamIndex=0;ParamIndex < CurrentParamList->Count;ParamIndex++)
      {
         CurrentParam = dynamic_cast<TParamObj*>(CurrentParamList->Objects[ParamIndex]);
         if(CurrentParam == NULL) continue;
         FormatTagStr = "<"+CurrentParamList->Strings[ParamIndex];
         switch(CurrentParam->TypeID)
         {
         case TI_INT:
            FormatTagStr += ">"+IntToStr(CurrentParam->Value);
            break;
         case TI_STRING:
            FormatTagStr += ">"+CurrentParam->Text;
            break;
         case TI_LIST:
            FormatTagStr += ">"+CurrentParam->Text;
            break;
         case TI_FILE:
            FormatTagStr += ">"+CurrentParam->Text;
            break;
         case TI_FONT:
            FormatTagStr += " Size=\""+IntToStr(CurrentParam->Font->Size)+"\"";
//            FormatTagStr += " Style=\""+IntToStr(CurrentParam->Font->Style)+"\"";
            FormatTagStr += " Color=\""+IntToStr((int)CurrentParam->Font->Color)+"\"";
            FormatTagStr += ">"+CurrentParam->Font->Name;
            break;
         };
         FormatTagStr += "</"+CurrentParamList->Strings[ParamIndex]+">";
         //Сохранить в файл значение параметра
         SaveFile->Add(FormatTagStr);
      }
      SaveFile->Add("</"+CurrentParamList->CompType+">");
   }
   //Добавить завершающий код
   SaveFile->Add("</"+ProjectName+">");
   //Сохаринть файл
   SaveFile->SaveToFile(SaveProjectDlg->FileName);
   delete SaveFile;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MI_OpenPrjClick(TObject *Sender)
{//Функция открытия проекта
   //Вызвать диалог выбора файла проекта
   if(OpenProjectDlg->Execute() == false) return;
   FlashProject();
   StatusBar1->Panels->Items[0]->Text = OpenProjectDlg->FileName;
   //Открыть файл проекта для разбора
   _di_IXMLDocument MyXmlDoc = LoadXMLDocument(OpenProjectDlg->FileName);
   //Получить корневую запись
   _di_IXMLNode RootNode = MyXmlDoc->GetDocumentElement();
   //Получить значение фонового изображения из атрибута корнеовй записи
   SurfaceFileName = RootNode->GetAttribute("Surface");
   if(SurfaceFileName != "")
   {//Загурзить изображение
      LoadSurface(SurfaceFileName);
   }
   //Получить указатель на список загружаемых элементов
   _di_IXMLNodeList CompObjectList = RootNode->GetChildNodes();
   //Получить количество загружаемых элементов
   int LoadObjectCount = CompObjectList->Count;
   //Выполнить загрузку элементов на рабочую область
   for(int LoadObjectIndex=0;LoadObjectIndex < LoadObjectCount;LoadObjectIndex++)
   {
      CreateCompObject(CompObjectList->Get(LoadObjectIndex));
   }
   //Обновить форму
   WorkArea->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CreateCompObject(_di_IXMLNode CurrentObject)
{//Созадать комонент объекта
   TCustomKit *ElementParam;
   AnsiString ElementName;
   //Получить тип объекта
   AnsiString CompType = CurrentObject->GetNodeName();
   //Получить список параметров
   _di_IXMLNodeList CompParamList = CurrentObject->GetChildNodes();
   //Создать объект в зависимости от типа
   AnsiString StrValue;
   if(CompType == "TImageKit")
   {//Создать элемент "изображене"
      TImageKit *IK = new TImageKit(WorkArea);
      //Установить обработчики событий при выделении и изменении состояния
      IK->OnChangePos = LabelKit1Change;
      IK->OnSelect    = LabelKit1Select;
      IK->Left = StrToInt(CompParamList->FindNode("Left")->Text);
      IK->Top  = StrToInt(CompParamList->FindNode("Top")->Text);
      IK->Height = StrToInt(CompParamList->FindNode("Height")->Text);
      IK->Width = StrToInt(CompParamList->FindNode("Width")->Text);
      IK->Parent = WorkArea; //Отобразить элемент на области
      IK->Name = CompParamList->FindNode("Name")->Text;
      if(CompParamList->FindNode("Image")->IsTextElement)
         IK->ImageFileName = CompParamList->FindNode("Image")->Text;
      IK->Tag = 0xFF; //Установить признак компонента
      ElementParam = IK->Params;
      ElementName  = IK->Name;
      ElementParam->GetParams();
      ((TParamObj*)ElementParam->Objects[ElementParam->IndexOf("Stretch")])->Text =
         IK->ImageFileName = CompParamList->FindNode("Stretch")->Text;
      ElementParam->SetParams();
   }

   if(CompType == "TLabelKit")
   {//Создать элемент "текст"
      TLabelKit *LK = new TLabelKit(WorkArea);
      //Установить обработчики событий при выделении и изменении состояния
      LK->OnChangePos = LabelKit1Change;
      LK->OnSelect    = LabelKit1Select;
      LK->Left = StrToInt(CompParamList->FindNode("Left")->Text);
      LK->Top  = StrToInt(CompParamList->FindNode("Top")->Text);
      LK->Height = StrToInt(CompParamList->FindNode("Height")->Text);
      LK->Width = StrToInt(CompParamList->FindNode("Width")->Text);
      LK->Parent = WorkArea; //Отобразить элемент на области
      LK->Name = CompParamList->FindNode("Name")->Text;
      LK->Caption = CompParamList->FindNode("Text")->Text;
      LK->Tag = 0xFF; //Установить признак компонента
      //Устанвоить параметры шрифта
      LK->Font->Name  = CompParamList->FindNode("Font")->Text;
      LK->Font->Size  = StrToInt(CompParamList->FindNode("Font")->GetAttribute("Size"));
      LK->Font->Color = (TColor)StrToInt(CompParamList->FindNode("Font")->GetAttribute("Color"));
      ElementParam = LK->Params;
      ElementName  = LK->Name;
      ElementParam->GetParams();
      ElementParam->SetParams();
   }

   //Добавить новый элемент в список объектов
   CB_SelectObjectList->Items->AddObject(ElementName,ElementParam);
   CB_SelectObjectList->ItemIndex = CB_SelectObjectList->Items->Count - 1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MI_PrintClick(TObject *Sender)
{//Выполнить печать изображения
   //Выровнить все элементы по верхнему и левому караю
   ResyncWorkArea();
   //Созадать область для печати
   Graphics::TBitmap *PrintImage = new Graphics::TBitmap();
   //Сформировать изображение
   CreateCompImage(PrintImage,false);
   //
   TMemoryStream *MS = new TMemoryStream();
   PrintImage->SaveToStream(MS);
   MS->Position = 0;
   PrintImage->LoadFromStream(MS);
   delete MS;
   //Печать изображения на карту
   TPrinter *Prntr = Printer();
   PrintDialog1->Execute();
   Prntr->BeginDoc();

   //Установить размеры листа
   Graphics::TBitmap *Bmp = new Graphics::TBitmap();
   Bmp->Width  = 2315;
   Bmp->Height = 3231;
   //Очистить поле дукумента
   Bmp->FreeImage();
   Bmp->Canvas->CopyRect(TRect(80,80,1930+160,(2590/2)+160),PrintImage->Canvas,
                       TRect(80,80,PrintImage->Width,PrintImage->Height));
   Prntr->Canvas->StretchDraw(TRect(0,0,Prntr->PageWidth,Prntr->PageHeight),Bmp);
   delete Bmp;

   //Выполнить печать документа
//   Prntr->Canvas->Draw(0,0,PrintImage);
   Prntr->EndDoc();
   delete PrintImage;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ResyncWorkArea()
{//Выровнить все элементы по верхнему и левому караю
   WorkArea->VertScrollBar->Position = 0;
   WorkArea->HorzScrollBar->Position = 0;
   for(int i=0;i < CB_SelectObjectList->Items->Count;i++)
   {
      ((TCustomKit*)CB_SelectObjectList->Items->Objects[i])->Selected = true;
      ((TCustomKit*)CB_SelectObjectList->Items->Objects[i])->Selected = false;
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MI_SaveImageClick(TObject *Sender)
{//Сохранить изображение
   if(SaveImageDlg->Execute() == false) return;
   Graphics::TBitmap *SaveImage = new Graphics::TBitmap();
   //Сформировать изображение
   CreateCompImage(SaveImage,true);
   //Сохранить изображение
   SaveImage->SaveToFile(SaveImageDlg->FileName);
   //Вызвать диалог для сохранения изображения
   delete SaveImage;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CreateCompImage(Graphics::TBitmap *Aimage,bool DrawFon)
{//Создать изображение компонента
   ResyncWorkArea();
   Aimage->Width  = Image1->Width;
   Aimage->Height = Image1->Height;
   Graphics::TBitmap *CompBitmap;
   TCustomKit *CompElement;
   int CompPosLeft,CompPosTop;
   int CompIndex;
   //Добавить фон изображения
   if(DrawFon)
      Aimage->Canvas->Draw(0,0,Image1->Picture->Graphic);
   //Отобразить на канве все компоненты из списка
   for(int i=0;i < CB_SelectObjectList->Items->Count;i++)
   {
      CompElement = ((TCustomKit*)CB_SelectObjectList->Items->Objects[i]);
      CompBitmap = CompElement->GetBitmap();
      //Получить координаты для печати
      CompIndex = CompElement->IndexOf("Left");
      if(CompIndex != -1)
         CompPosLeft = ((TParamObj*)CompElement->Objects[CompIndex])->Value;
      CompIndex = CompElement->IndexOf("Top");
      if(CompIndex != -1)
         CompPosTop = ((TParamObj*)CompElement->Objects[CompIndex])->Value;

      Aimage->Canvas->Draw(CompPosLeft,CompPosTop,CompBitmap);
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MI_DeleteClick(TObject *Sender)
{//Удалить выделенный элемент из списка
   if(SelectObject == NULL) return;
   int iIndex = CB_SelectObjectList->ItemIndex;
   CB_SelectObjectList->Items->Objects[iIndex] = NULL;
   CB_SelectObjectList->Items->Delete(iIndex);
   TComponent *Item = SelectObject->CompOwner;
   SelectObject = NULL;
   WorkArea->RemoveComponent(Item);
   delete Item;
   if(CB_SelectObjectList->Items->Count != 0)
   {
      CB_SelectObjectList->ItemIndex = 0;
   }
   else
   {
      CB_SelectObjectList->Items->Clear();
      SG_ObjectInspector->RowCount = 1;
      SG_ObjectInspector->Cells[0][0] = "";
      SG_ObjectInspector->Cells[1][0] = "";
   }
   //Выделить 0-й элемент из списка
   CB_SelectObjectListChange(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FlashProject()
{//Функция "сброса" проекта
   //Удалить все элементы из списка
   TComponent *Item;
   SelectObject = NULL;
   while(CB_SelectObjectList->Items->Count != 0)
   {
      Item = ((TCustomKit*)CB_SelectObjectList->Items->Objects[0])->CompOwner;
      CB_SelectObjectList->Items->Objects[0] = NULL;
      CB_SelectObjectList->Items->Delete(0);
      WorkArea->RemoveComponent(Item);
      delete Item;
   }
   CB_SelectObjectList->Items->Clear();
   SG_ObjectInspector->RowCount = 1;
   SG_ObjectInspector->Cells[0][0] = "";
   SG_ObjectInspector->Cells[1][0] = "";
}
//---------------------------------------------------------------------------

