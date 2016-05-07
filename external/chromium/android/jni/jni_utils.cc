 // Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "android/jni/jni_utils.h"
#include "base/logging.h"
#include "base/utf_string_conversions.h"
#include "jni.h"

#ifdef SEC_PRODUCT_FEATURE_RIL_ENABLE_SMART_BONDING
//< RNTFIX:: JNI Support for streaming
#include <dlfcn.h>
#include <pthread.h>
extern "C" jint Java_com_android_internal_util_WithFramework_registerNatives(
        JNIEnv* env, jclass clazz);
//> RNTFIX
#endif


namespace {

JavaVM* sVM = NULL;
//< RNTFIX:: JNI Support for streaming
#ifdef SEC_PRODUCT_FEATURE_RIL_ENABLE_SMART_BONDING
static JavaVM* streamVM = NULL;
static bool created = false;
static JNIEnv* penv = NULL;
static pthread_mutex_t mut;
#endif
//> RNTFIX
JavaVM* getJavaVM() {
  return sVM;
}
#ifdef SEC_PRODUCT_FEATURE_RIL_ENABLE_SMART_BONDING
//< RNTFIX:: JNI Support for streaming
JavaVM* getNewJavaVM() {
  return streamVM;
}
JavaVM* GetStreamJavaVM()
{
if(!streamVM)
{
    // start a VM
    JavaVMInitArgs args;
    JavaVMOption opt;

    opt.optionString =
        "-agentlib:jdwp=transport=dt_android_adb,suspend=n,server=y";

    args.version = JNI_VERSION_1_4;
    args.options = &opt;
    args.nOptions = 1;
    args.ignoreUnrecognized = JNI_FALSE;

#if 1
    void* libdvm_dso = dlopen("libdvm.so", RTLD_NOW);

    void* libandroid_runtime_dso = dlopen("libandroid_runtime.so", RTLD_NOW);

    if (!libdvm_dso || !libandroid_runtime_dso) 
    {
      //LOG(ERROR)<< "libdvm_dso or libandroid_runtime_dso open fail";
      if(libdvm_dso)
      {
        dlclose(libdvm_dso);
      }
      if(libandroid_runtime_dso)
      {
        dlclose(libandroid_runtime_dso);
      }
      libdvm_dso = NULL;
      libandroid_runtime_dso = NULL;
      created = true;
      return 0;
    }

    jint (*JNI_CreateJavaVM)(JavaVM** p_vm, JNIEnv** p_env, void* vm_args);
    JNI_CreateJavaVM = (typeof JNI_CreateJavaVM)dlsym(libdvm_dso, "JNI_CreateJavaVM");

    jint (*registerNatives)(JNIEnv* env, jclass clazz);
    registerNatives = (typeof registerNatives)dlsym(libandroid_runtime_dso,
        "Java_com_android_internal_util_WithFramework_registerNatives");

    if (!JNI_CreateJavaVM || !registerNatives) 
    {
      if(libdvm_dso)
      {
        dlclose(libdvm_dso);
      }
      if(libandroid_runtime_dso)
      {
        dlclose(libandroid_runtime_dso);
      }
      libdvm_dso = NULL;
      libandroid_runtime_dso = NULL;	
      created = true;
      return 0;
    }
#endif
    if(JNI_CreateJavaVM(&streamVM, &penv, &args) == 0)
    {
//        LOG(ERROR) << " JNI_CreateJavaVM successs ful VM address" << streamVM; 
    }
#if 0
    if (registerNatives(penv, 0) == 0) {
        //LOG(ERROR) << "registerNatives success" << penv;
    }
#endif
    if (Java_com_android_internal_util_WithFramework_registerNatives(penv, 0) == 0)
    {
        //LOG(ERROR) << "registerNatives success" << penv;
    }
    if(libdvm_dso)
    {
      dlclose(libdvm_dso);
    }
    if(libandroid_runtime_dso)
    {
      dlclose(libandroid_runtime_dso);
    }
    libdvm_dso = NULL;
    libandroid_runtime_dso = NULL;
    created = true;
    return streamVM;
  }
  else
    return streamVM;
}
void * createNewVM(void *)
{
    while(1)
    {
        // if first loop create it and next loop it should not create VM again
        if (!getNewJavaVM())
        {
            if (GetStreamJavaVM() == 0)
                break;
        }
        //LOG(ERROR) << "VM created now"<<getNewJavaVM();
        select(0,0,0,0,0);
        //LOG(ERROR) << "VM created nd select call donei";
    }
    return 0;
}
//> RNTFIX
#endif
}

namespace android {
namespace jni {

// All JNI code assumes this has previously been called with a valid VM
void SetJavaVM(JavaVM* vm)
{
  sVM = vm;
#ifdef SEC_PRODUCT_FEATURE_RIL_ENABLE_SMART_BONDING
//< RNTFIX:: JNI Support for streaming
    if(!sVM)
    {
      pthread_mutex_lock(&mut);
      if ((streamVM == NULL) && (false == created))
      {
        pthread_t ThreadId; 
        pthread_attr_t        attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if(0 != pthread_create(&ThreadId,NULL,&createNewVM,0))
        {
          LOG(ERROR)<<"Call Back Thread Failed ";
        }/* End of if */
        while((streamVM == NULL) && (created == false)) 
        {
          //LOG(ERROR)<<"creating vm present status "<<created << " " <<streamVM;
          usleep(100000);
        }
      }
      pthread_mutex_unlock(&mut);
    }
//> RNTFIX
#endif
}

bool checkException(JNIEnv* env)
{
  if (env->ExceptionCheck() != 0)
  {
    LOG(ERROR) << "*** Uncaught exception returned from Java call!\n";
    env->ExceptionDescribe();
    return true;
  }
  return false;
}

string16 jstringToString16(JNIEnv* env, jstring jstr)
{
  if (!jstr || !env)
    return string16();

  const char* s = env->GetStringUTFChars(jstr, 0);
  if (!s)
    return string16();
  string16 str = UTF8ToUTF16(s);
  env->ReleaseStringUTFChars(jstr, s);
  checkException(env);
  return str;
}

std::string jstringToStdString(JNIEnv* env, jstring jstr)
{
  if (!jstr || !env)
    return std::string();

  const char* s = env->GetStringUTFChars(jstr, 0);
  if (!s)
    return std::string();
  std::string str(s);
  env->ReleaseStringUTFChars(jstr, s);
  checkException(env);
  return str;
}

JNIEnv* GetJNIEnv() {
  JNIEnv* env;
#ifdef SEC_PRODUCT_FEATURE_RIL_ENABLE_SMART_BONDING
 if(getJavaVM() == 0)
  {
    JavaVM * vm  = getNewJavaVM();
    if (vm)
    {
      vm->AttachCurrentThread(&env, NULL);
        return env;
    }
    else
      return 0;
  }
  else
#endif
  getJavaVM()->AttachCurrentThread(&env, NULL);
  return env;
}

std::string JstringToStdString(JNIEnv* env, jstring jstr) {
  return jstringToStdString(env, jstr);
}

string16 JstringToString16(JNIEnv* env, jstring jstr)
{
    return jstringToString16(env, jstr);
}

bool CheckException(JNIEnv* env)
{
    return checkException(env);
}

jstring ConvertUTF8ToJavaString(JNIEnv* env, std::string str)
{
    return env->NewStringUTF(str.c_str());
}

void DetachFromVM()
{
    JavaVM* vm = getJavaVM();
//< RNTFIX:: JNI Support for streaming
    if (vm)
    vm->DetachCurrentThread();
#ifdef SEC_PRODUCT_FEATURE_RIL_ENABLE_SMART_BONDING
    else if(getNewJavaVM())
        getNewJavaVM()->DetachCurrentThread();
//> RNTFIX		
#endif
}

} // namespace jni
} // namespace android

