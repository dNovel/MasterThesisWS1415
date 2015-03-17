                                   // This code is for learning purposes only
                       //Note that this code uses unregistered ID numbers for testing purposes only


#define PLUGIN_ID 1000006  // be sure to use a unique ID obtained from www.plugincafe.com

#include "c4d.h"
#include "c4d_symbols.h"
#include "c4d_basedata.h"
#include "..\..\..\modules\mograph\res\description\otransform_panel.h"
#include "..\..\..\modules\mograph\res\description\omograph_text.h"
#include "ge_dynamicarray.h"

class myDialog : public GeDialog
{
	private:
		BitmapButtonCustomGui *myButton;
		myDialog* dlg;

	public:
		myDialog(void);
		~myDialog(void);
		virtual Bool CreateLayout(void);
		virtual Bool InitValues(void);
		virtual Bool Command(LONG id,const BaseContainer &msg);
		virtual LONG Message(const BaseContainer &msg,BaseContainer &result);
		virtual Bool GetDDescription(GeListNode *node, Description *description,DESCFLAGS_DESC &flags);		
};


myDialog::myDialog(void)
{	
}

myDialog::~myDialog(void)
{	
	GeFree(dlg);
}


enum
{
	MY_COMBOBUTTON2 = 5000,  //This is used here instead of the .res file because we created this button in this .cpp file without using the AddComboBox() method
	_dummy
};

Bool myDialog::CreateLayout(void)
{

	Bool res = TRUE;
    res = LoadDialogResource(IDS_RESDIALOG,NULL,0);

	//This text box we'll add here in this .cpp file. And not from the external .res file
    GroupBegin(0,BFH_LEFT,2,0,"MyTextGroup",0);
			{				
			AddEditText(1001, BFH_SCALEFIT,400,15,0);
			}
    GroupEnd();
	
	//This combobutton we'll add here in this .cpp file. And not from the external .res file
    GroupBegin(0,BFH_LEFT,2,0,"MyGroup",0);
			{				
			AddComboBox(MY_COMBOBUTTON2, BFH_SCALEFIT);
				IconData dat1,dat2,dat3,dat4;  //Create a few new IconData type variables to hold our icons
				GetIcon(Ocube,&dat1);
				GetIcon(Osphere,&dat2);
				GetIcon(Ocylinder,&dat3);     //Assign some icons to the variables we created above
				GetIcon(Ttexture,&dat4);      //IconID's can be found in the file CINEMA 4D R12/resource/libs/interface_icons.txt
									
				AddChild(MY_COMBOBUTTON2, 0, GeLoadString(IDS_CUBE)+"&i"+LongToString(Ocube)+"&"); //IDS_CUBE is declared in the c4d_symbols.h file & the text is in c4d_strings.str file 
				AddChild(MY_COMBOBUTTON2, 1, GeLoadString(IDS_PHONG)+"&i"+LongToString(Tphong)+"&");				
				AddChild(MY_COMBOBUTTON2, 2, GeLoadString(IDS_GRADIENT)+"&i"+LongToString(200000135)+"&"); //gradient icon ID
				AddChild(MY_COMBOBUTTON2, 3, GeLoadString(IDS_SPHERE)+"&"+PtrToString(&dat2)+"&"); //This style of loading an icon uses a pointer to find the icon image. Instead of using the above methods
				                                                                                   //You can decide which one you prefer
			}
    GroupEnd();

	//This is an example of creating a custom button with AddCustomGui()
    GroupBegin(0,BFH_LEFT,2,0,"MySecondGroup",0);
			{
	        BaseContainer bbc;                             //Create a container to hold our custom button gizmo	        
	        bbc.SetLong(BITMAPBUTTON_BORDER, BORDER_OUT);  //Make the button style raised so it looks like a button
	        bbc.SetBool(BITMAPBUTTON_BUTTON, TRUE);	
	        myButton = (BitmapButtonCustomGui*)AddCustomGui(10001,CUSTOMGUI_BITMAPBUTTON,"MY BUTTON", 0L,80,80,bbc); //Adds the BitmapButton GUI to the dialog	
	        //myButton->SetImage(Osphere, FALSE); //Adds the sphere icon to the button if that's what you want
	        myButton->SetImage(200000000, FALSE); //Adds our custom registered icon using it's ID#...Be sure to get a real ID# from c4dcafe.com. DON'T use this one in your public projects!
			}
    GroupEnd();

	return res;
}


Bool myDialog::InitValues(void)
{
	// first call the parent instance
	if (!GeDialog::InitValues()) return FALSE;

	this->SetBool(MY_CHECKBOX,FALSE);           //Sets the checkbox to enabled by default-->looks in the description->c4d_symbols.h file for matching name
	this->SetReal(MY_NUMARROWS,0);              //Sets the EDITNUMBERARROWS gizmo to zero and also lets it's values increase or decrease
	this->SetLong(MY_COMBOBUTTON,FIRST_CHILD);  //Sets the initial button value to the first option in the list-->looks in the description->c4d_symbols.h file for matching name
	this->SetString(1001,"Hello");

	return TRUE;
}



Bool myDialog::Command(LONG id,const BaseContainer &msg)  //This is where the code that does something goes
{
	BaseDocument *doc = GetActiveDocument(); //Get the active document

	AutoAlloc<BaseBitmap> myicon;
	IconData idata;                    //Create an IconData type variable to hold our custom registered icon's ID#
    idata.w = myicon->GetBw();         //Get the width of the Icon if needed later on
	idata.h = myicon->GetBh();         //Get the height of the Icon if needed later on

    GetIcon(200000000,&idata);        //Get our custom registered icon. The ID# is set up in the "RegistermyResDialog(void)" function
	GetCustomIconData *myIconData =((GetCustomIconData*)&idata); //Create a CustomIconData type variable called "myicon"	
    myIconData->filled=TRUE;          //Tells C4D that the state of the icon is "filled"


	//Set up some actions that will tell c4d that a gizmo has been triggered..We'll used those action variables later on in the switch code block
	LONG myComboButFirst = msg.GetLong(BFM_ACTION_VALUE);	//Assigns an action to a variable      
    LONG myComboButSecond = msg.GetLong(BFM_ACTION_VALUE);	//Assigns an action to a variable

	LONG myComboBut2First = msg.GetLong(BFM_ACTION_VALUE);	//Assigns an action to a variable
	LONG myComboBut2Second = msg.GetLong(BFM_ACTION_VALUE);	//Assigns an action to a variable
	LONG myComboBut2Third = msg.GetLong(BFM_ACTION_VALUE);	//Assigns an action to a variable
	LONG myComboBut2Fourth = msg.GetLong(BFM_ACTION_VALUE);	//Assigns an action to a variable





	switch (id) 
      {
      case MY_BUTTON:		     
		     GePrint("Button Was Pressed");			 
			 GePrint(LongToString(idata.w)); //Print the icon image's width
	         GePrint(LongToString(idata.h)); //Print the icon image's height
			 if (!GetIcon(200000000,&idata)) GePrint("icon error"); //Only prints if there was an error loading the icon image

            break;

      case MY_CHECKBOX:          
            GePrint("CHKBox was Toggled");
            break;

      case MY_COMBOBUTTON:	//This button was created externally in the .res file				
			if(myComboButFirst == FIRST_CHILD) GePrint("First Option Selected");
			if(myComboButSecond == SECOND_CHILD) GePrint("Second Option Selected");
			break;


	  case MY_COMBOBUTTON2:					
			if(myComboBut2First == 0) GePrint("First Option Selected");
			if(myComboBut2Second == 1) GePrint("Second Option Selected");
			if(myComboBut2Third == 2) GePrint("Third Option Selected");
			if(myComboBut2Fourth == 3) GePrint("Fouth Option Selected");			
			break;

	  case 10001:
		    GePrint("Custom GUI Button Was Pressed");			
            break;
	   }

    EventAdd();

	return TRUE;
}

LONG myDialog::Message(const BaseContainer &msg, BaseContainer &result)
{
    String lastWord;  //This will be the last word that is typed into a text box

  switch(msg.GetId())
     {

       case BFM_INPUT:   //A dialog/userarea receives this message if any mouse or keyboard input is received

          if(msg.GetLong(BFM_INPUT_DEVICE) == BFM_INPUT_KEYBOARD) //If the input is from the keyboard
           {
             String input = msg.GetString(BFM_INPUT_ASC);     //Create a string type variable... 
             GePrint(input);                                  //and assign it to the pressed key's unicode-text value
			 if(input == ".") 
			 {
				//GePrint("You pressed the period key");
				String text;  this->GetString (1001, text);   //Get the text in the text box
				//GePrint(text);

				LONG length = text.GetLength();
				LONG pos = NULL;                              //The variable that will hold the postion of the string
				Bool lastSpace = text.FindLast(" ",&pos, -1); //Find the last blank space and store it's position in the string
				//GePrint(LongToString(lastSpace));           //Yes or no if it finds it
				//GePrint(LongToString(pos));                 //The position

				String lw = text.SubStr(pos+1, length - pos);  //Grab the last word (sub string)
                lastWord = lw;                               //Assign the value to another variable that's scope isn't locked up inside this loop
			 }
           }

          break;
     }  //End the key pressed case loop /////////////////////////

    GePrint(lastWord);


     return GeDialog::Message(msg,result);
}

Bool myDialog::GetDDescription(GeListNode *node, Description *description,DESCFLAGS_DESC &flags)
{	
    //not used
    return TRUE;
} 

class myResDialog : public CommandData // myResDialog is the class name that needs to be listed in the main.cpp file to register it properly 
{
	private:
		myDialog dlg;

	public:
		virtual Bool Execute(BaseDocument *doc);
		virtual LONG GetState(BaseDocument *doc);
		virtual Bool RestoreLayout(void *secret);		
};

LONG myResDialog::GetState(BaseDocument *doc)
{
	return CMD_ENABLED;
}

Bool myResDialog::Execute(BaseDocument *doc)
{
	StopAllThreads();
	//return dlg.Open(DLG_TYPE_ASYNC_FULLSCREEN_MONITOR,ID_PLUGIN,0,0); //Use this for full screen if desired
	return dlg.Open(DLG_TYPE_ASYNC,PLUGIN_ID, -1, -1, 300,150);
}

Bool myResDialog::RestoreLayout(void *secret)
{
	return dlg.RestoreLayout(PLUGIN_ID,0,secret);
}

Bool RegistermyResDialog(void)
{	     
    Filename fn = GeGetPluginPath()+"res"+"myicon.png";      //The file path where the icon image is located 
    AutoAlloc<BaseBitmap> bmp;                               //Create a BaseBitmap variable...Images are loaded via an instance of the the BaseBitmap class
    if (IMAGERESULT_OK != bmp->Init(fn)) return FALSE;       //Don't register the plugin if the image is not present	
    RegisterIcon(200000000,bmp,0*32,0,32,32, ICONFLAG_COPY); //Register our custom icon image with c4d so we can use it like other icons
	                                                         //NOTE:The ID# 200000000 is for testing purposes only!!...Be sure to get a proper one from c4dcafe.com  

    String Help = "C++ Dialog using external resources"; //This string appears in the status bar when the user hovers over the plugin name in the menu

    //Register the plugin
    return RegisterCommandPlugin(PLUGIN_ID, "C++ RES Based Dialog", 0, AutoBitmap("icon.tif"),Help, gNew myResDialog);
}
