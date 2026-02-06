#include "torrent_jni.h"

#include <libtorrent/session.hpp>
#include <libtorrent/settings_pack.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/add_torrent_params.hpp>

#include <memory>

using namespace lt;

extern "C" {

// Create a high-performance libtorrent session
JNIEXPORT jlong JNICALL
Java_com_tech_piratebay_NativeTorrent_nativeCreateSession(
        JNIEnv *, jobject) {

    settings_pack settings;
    settings.set_int(settings_pack::alert_mask,
        alert_category::status |
        alert_category::error |
        alert_category::storage);

    settings.set_bool(settings_pack::enable_dht, true);
    settings.set_bool(settings_pack::enable_lsd, true);
    settings.set_bool(settings_pack::enable_upnp, false);
    settings.set_bool(settings_pack::enable_natpmp, false);

    settings.set_int(settings_pack::connections_limit, 200);
    settings.set_int(settings_pack::download_rate_limit, 0);
    settings.set_int(settings_pack::upload_rate_limit, 0);

    auto *sess = new session(settings);
    return reinterpret_cast<jlong>(sess);
}

// Start magnet streaming
JNIEXPORT void JNICALL
Java_com_tech_piratebay_NativeTorrent_nativeStartMagnet(
        JNIEnv *env,
        jobject,
        jlong sessionPtr,
        jstring magnetUri,
        jstring savePath) {

    if (!sessionPtr) return;

    auto *sess = reinterpret_cast<session *>(sessionPtr);

    const char *magnet = env->GetStringUTFChars(magnetUri, nullptr);
    const char *path   = env->GetStringUTFChars(savePath, nullptr);

    add_torrent_params params;
    params.save_path = path;

    error_code ec;
    parse_magnet_uri(magnet, params, ec);

    if (!ec) {
        sess->add_torrent(std::move(params));
    }

    env->ReleaseStringUTFChars(magnetUri, magnet);
    env->ReleaseStringUTFChars(savePath, path);
}

// Stop and destroy session
JNIEXPORT void JNICALL
Java_com_tech_piratebay_NativeTorrent_nativeStopSession(
        JNIEnv *, jobject, jlong sessionPtr) {

    if (!sessionPtr) return;

    auto *sess = reinterpret_cast<session *>(sessionPtr);
    delete sess;
}

}
