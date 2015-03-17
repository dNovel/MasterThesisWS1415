CONTAINER tsimpletag
{
	NAME tsimpletag;
	//INCLUDE Texpression;  //used for priority GUI if desired
	//INCLUDE Obase;
	
	
  GROUP
	{	
	  BOOL MYBOX { }

	  SEPARATOR { LINE; }

	  REAL POLYHUDX { MIN 0.0; UNIT METER; }
	  REAL POLYHUDY { MIN 0.0; UNIT METER; }

	  SEPARATOR { LINE; }

	  REAL POINTHUDX { MIN 5.0; UNIT METER; }
	  REAL POINTHUDY { MIN 0.0; UNIT METER; }

	}
}
