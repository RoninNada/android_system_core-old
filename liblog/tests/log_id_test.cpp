/*
 * Copyright (C) 2013-2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include <gtest/gtest.h>
// Test the APIs in this standalone include file
#include <log/log_id.h>

// We do not want to include <android/log.h> to acquire ANDROID_LOG_INFO for
// include file API purity.  We do however want to allow the _option_ that
// log/log_id.h could include this file, or related content, in the future.
#ifndef __android_LogPriority_defined
# define ANDROID_LOG_INFO 4
#endif

TEST(liblog, log_id) {
#ifdef __ANDROID__
    int count = 0;

    for(int i = LOG_ID_MIN; i < LOG_ID_MAX; ++i) {
        log_id_t id = static_cast<log_id_t>(i);
        const char *name = android_log_id_to_name(id);
        if (id != android_name_to_log_id(name)) {
            continue;
        }
        ++count;
        fprintf(stderr, "log buffer %s\r", name);
    }
    ASSERT_EQ(LOG_ID_MAX, count);
#else
    GTEST_LOG_(INFO) << "This test does nothing.\n";
#endif
}

TEST(liblog, __android_log_buf_print) {
#ifdef __ANDROID__
    EXPECT_LT(0, __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_print",
                                         "radio"));
    usleep(1000);
    EXPECT_LT(0, __android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_print",
                                         "system"));
    usleep(1000);
    EXPECT_LT(0, __android_log_buf_print(LOG_ID_MAIN, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_print",
                                         "main"));
    usleep(1000);
#else
    GTEST_LOG_(INFO) << "This test does nothing.\n";
#endif
}

TEST(liblog, __android_log_buf_write) {
#ifdef __ANDROID__
    EXPECT_LT(0, __android_log_buf_write(LOG_ID_RADIO, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_write",
                                         "radio"));
    usleep(1000);
    EXPECT_LT(0, __android_log_buf_write(LOG_ID_SYSTEM, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_write",
                                         "system"));
    usleep(1000);
    EXPECT_LT(0, __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_write",
                                         "main"));
    usleep(1000);
#else
    GTEST_LOG_(INFO) << "This test does nothing.\n";
#endif
}

#ifdef __ANDROID__
static void* ConcurrentPrintFn(void *arg) {
    int ret = __android_log_buf_print(LOG_ID_MAIN, ANDROID_LOG_INFO,
                                  "TEST__android_log_print", "Concurrent %" PRIuPTR,
                                  reinterpret_cast<uintptr_t>(arg));
    return reinterpret_cast<void*>(ret);
}
#endif

#define NUM_CONCURRENT 64
#define _concurrent_name(a,n) a##__concurrent##n
#define concurrent_name(a,n) _concurrent_name(a,n)

TEST(liblog, concurrent_name(__android_log_buf_print, NUM_CONCURRENT)) {
#ifdef __ANDROID__
    pthread_t t[NUM_CONCURRENT];
    int i;
    for (i=0; i < NUM_CONCURRENT; i++) {
        ASSERT_EQ(0, pthread_create(&t[i], NULL,
                                    ConcurrentPrintFn,
                                    reinterpret_cast<void *>(i)));
    }
    int ret = 0;
    for (i=0; i < NUM_CONCURRENT; i++) {
        void* result;
        ASSERT_EQ(0, pthread_join(t[i], &result));
        int this_result = reinterpret_cast<uintptr_t>(result);
        if ((0 == ret) && (0 != this_result)) {
            ret = this_result;
        }
    }
    ASSERT_LT(0, ret);
#else
    GTEST_LOG_(INFO) << "This test does nothing.\n";
#endif
}