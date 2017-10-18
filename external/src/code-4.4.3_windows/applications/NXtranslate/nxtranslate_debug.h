// define default debug level
#ifdef DEBUG_NXTRANSLATE
#define DEBUG1_NXTRANSLATE
#endif

// ----- level three debugging
#ifdef DEBUG3_NXTRANSLATE
#define DEBUG2_NXTRANSLATE
#define DEBUG3_NEXUS_UTIL
#define DEBUG3_XML_PARSER
#endif

// ----- level two debugging
#ifdef DEBUG2_NXTRANSLATE
#define DEBUG1_NXTRANSLATE
#define DEBUG2_NEXUS_UTIL
#define DEBUG2_XML_PARSER
#endif

// ----- level one debugging
#ifdef DEBUG1_NXTRANSLATE
#define DEBUG1_NEXUS_UTIL
#define DEBUG1_XML_PARSER
#endif
