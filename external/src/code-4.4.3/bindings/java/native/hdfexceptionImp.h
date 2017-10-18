#ifndef HDFEXCEPTIONIMP
#define HDFEXCEPTIONIMP

extern jboolean outOfMemory( JNIEnv *env, char *functName);

extern jboolean JNIFatalError( JNIEnv *env, char *functName);

extern jboolean raiseException( JNIEnv *env, char *message);

#endif /* HDFEXCEPTIONIMP */
