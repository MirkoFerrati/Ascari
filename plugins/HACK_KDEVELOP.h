//HACK: this is usefull for developing with kdevelop, and is completely ignored by gcc

/*
#if !defined(ISAGENT) && !defined(ISSIMULATOR) 
#define ISVIEWER_KDEVELOP 1 
#endif 
#if !defined(ISVIEWER) && !defined(ISAGENT) 
#define ISSIMULATOR_KDEVELOP 1 
#endif 
#if !defined(ISVIEWER) && !defined(ISSIMULATOR) 
#define ISAGENT_KDEVELOP 1 
#endif 
#ifdef ISAGENT_KDEVELOP  
#define ISAGENT 1 
#endif
#ifdef ISVIEWER_KDEVELOP 
#define ISVIEWER 1 
#endif 
#ifdef ISSIMULATOR_KDEVELOP  
#define ISSIMULATOR 1 
#endif 

*/