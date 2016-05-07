#include <jni.h>
#include <JNIHelp.h>#include <string>
#include "base/logging.h"
#include "net/socket/ssl_client_socket_openssl.h"#include "net/http/http_auth_gssapi_posix.h"#include "net/url_request/url_request_job.h"

bool setOpenSslContext(JNIEnv *env, jobject obj, jboolean cac);bool registerCACEngine(JNIEnv *env, jobject obj);
void deregisterCACEngine(JNIEnv *env, jobject obj);
int getCACEngineState(JNIEnv *env, jobject obj);void setBrowserAppDataPath(JNIEnv *env, jobject obj, jstring path);int isNegotiateAuthSchemePresent(JNIEnv *env, jobject obj);
bool isTGTExpired(JNIEnv *env, jobject obj);
static JavaVM* gpVm = NULL;
static JNINativeMethod methods[] = {
  {"nativeSetOpenSslContext", "(Z)Z", (void*)&setOpenSslContext},  {"nativeRegisterCACEngine", "()Z", (void*)&registerCACEngine},  {"nativeDeregisterCACEngine", "()V", (void*)&deregisterCACEngine},  {"nativeGetCACEngineState", "()I", (void*)&getCACEngineState},  {"nativeSetBrowserAppDataPath", "(Ljava/lang/String;)V", (void*)&setBrowserAppDataPath},  {"nativeIsNegotiateAuthSchemePresent", "()I", (void*)&isNegotiateAuthSchemePresent},  {"nativeIsTGTExpired", "()Z", (void*)&isTGTExpired},
};

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
 gpVm = vm;
 JNIEnv* pEnv = NULL;
 gpVm->AttachCurrentThread(&pEnv,NULL);
 jniRegisterNativeMethods(pEnv, "android/webkitsec/JniUtil", methods, NELEM(methods));
 LOG(INFO) << "JNI_OnLoad";
 return JNI_VERSION_1_4;
}

bool setOpenSslContext(JNIEnv *env, jobject obj, jboolean cac) {
  return net::SSLClientSocketOpenSSL::setOpenSslContext(cac);
}
bool registerCACEngine(JNIEnv *env, jobject obj) {
  return net::SSLClientSocketOpenSSL::registerCACEngine();
}
void deregisterCACEngine(JNIEnv *env, jobject obj) {
  net::SSLClientSocketOpenSSL::deregisterCACEngine();
}
int getCACEngineState(JNIEnv *env, jobject obj) {
  return net::SSLClientSocketOpenSSL::getCACEngineState();
}void setBrowserAppDataPath(JNIEnv *env, jobject obj, jstring path) {  const char *nativePathString = env->GetStringUTFChars(path, 0);  std::string pathStr(nativePathString);  net::GSSAPISharedLibrary::setBrowserAppDataPath(pathStr);  env->ReleaseStringUTFChars(path, nativePathString);}int isNegotiateAuthSchemePresent(JNIEnv *env, jobject obj) {  return net::URLRequestJob::getNegotiateAuthState();}bool isTGTExpired(JNIEnv *env, jobject obj) {   return net::HttpAuthGSSAPI::isTGTExpired();} 
