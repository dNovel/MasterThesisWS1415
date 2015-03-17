CONTAINER tquicksteps
{
	NAME tquicksteps;
	INCLUDE Texpression;
	INCLUDE Obase;	
	
  GROUP ID_TAGPROPERTIES
    {
	 STATICTEXT MYTEXT {ANIM OFF; } //ANIM OFF hides the set key option from the user
     SEPARATOR { LINE; }
	 
      GROUP    //The master Group
      {
        LAYOUTGROUP; COLUMNS 3; 
  
        GROUP
        {
		  REAL NUMofGUIS {MIN 0.0; MAX 100.0; STEP 1;} //STEP controls the amount the values increase using the arrows
          BUTTON BTN_BAKE_STEPS {}
		  SEPARATOR { LINE; }		
		}
		
        GROUP
        {
		STATICTEXT DELETE_TEXT {ANIM OFF; }
		BUTTON BTN_DELETE_KEYS {}
		}

        GROUP
        {
		  REAL NUMofKEYS {MIN 1.0; MAX 100.0; STEP 1;} //STEP controls the amount the values increase using the arrows
          BUTTON BTN_BAKE_KEYS {}
        }
      }
    }
}