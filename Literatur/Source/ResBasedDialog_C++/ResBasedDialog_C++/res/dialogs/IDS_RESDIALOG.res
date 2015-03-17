	
  DIALOG IDS_RESDIALOG
  {
    NAME DIALOG_TITLE;  //The text in the dialog's window
  
    GROUP
    {
	  NAME TEXTGROUP;  
	  BORDERSTYLE BORDER_BLACK; BORDERSIZE -1, 1, 4, 4;	  	  
	  TITLECHECKBOX;
      COLUMNS 1;
	  ROWS 1;
      SPACE 4,4;
      BORDERSIZE 20,4,4,4;  
      STATICTEXT HELLO_WORLD{NAME HELLO_WORLD;}  //The NAME flag is linked to the entry in the IDS_RESDIALOG.str file to set the text
      }  	

    GROUP
    {
	  NAME BUTTONGROUP;
	  BORDERSTYLE BORDER_BLACK; BORDERSIZE -1, 1, 4, 4;	  
      COLUMNS 1;
      SPACE 4,4;
      BORDERSIZE 4,4,4,4;  
      BUTTON MY_BUTTON{ NAME MY_BUTTON; ALIGN_TOP; SCALE_H; SIZE 150, 5;} //The NAME flag is linked to the entry in the IDS_RESDIALOG.str file to set the text
      }  

    GROUP
    {
	  NAME MY_SEPARATOR;
	  BORDERSTYLE BORDER_GROUP_TOP; BORDERSIZE -1, 1, 4, 4;	  
      COLUMNS 1;
      SPACE 4,4;
      BORDERSIZE 4,4,4,4;  
      SEPARATOR MY_SEPARATOR{ NAME MY_SEPARATOR; ALIGN_TOP; SCALE_H; SIZE 200, 5;} //The NAME flag is linked to the entry in the IDS_RESDIALOG.str file to set the text
      }  

    GROUP
    {
	  NAME CHKGROUP;
	  BORDERSTYLE BORDER_BLACK; BORDERSIZE -1, 1, 4, 4;
      COLUMNS 1;
      SPACE 4,4;
      BORDERSIZE 90,4,4,4;
  
      CHECKBOX MY_CHECKBOX{ ALIGN_TOP; ALIGN_LEFT;  }  //The NAME flag is linked to the entry in the IDS_RESDIALOG.str file to set the text
      }

  GROUP IDC_STATIC
    {       
      BORDERSIZE 0, 0, 0, 0; 
      COLUMNS 2;
      SPACE 4, 4;      
      EDITNUMBERARROWS MY_NUMARROWS { CENTER_V; ALIGN_LEFT; SIZE 70, 0; } //The NAME flag is linked to the entry in the IDS_RESDIALOG.str file to set the text	   
    }

  GROUP IDC_STATIC
    {       
      BORDERSIZE 0, 0, 0, 0; 
      COLUMNS 2;
      SPACE 4, 4;      
      COMBOBOX MY_COMBOBUTTON    //MY_COMBOBUTTON is the button's ID	  
         {
          SCALE_H; SIZE 200,10; FIT_H;
          CHILDS           	  
           {
            FIRST_CHILD, FIRST_CHILD_TEXT;      //FIRST_CHILD is the first option's ID
            SECOND_CHILD, SECOND_CHILD_TEXT;    //SECOND_CHILD is the second option's ID            		
           }
         }   
    }
	  
    }
	
}