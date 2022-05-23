#pragma once
#include <string>
#include <wlanapi.h>


std::string getMacStdString(unsigned char* mac) {
    char mac_cstr[64];
    snprintf(mac_cstr, 64, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(mac_cstr);
}

std::string getAuthoAlgorithmString(DOT11_AUTH_ALGORITHM authAlgorithm) {
    std::string authString;
    switch (authAlgorithm) {
    case DOT11_AUTH_ALGO_80211_OPEN:
        authString.assign("802.11 Open");
        break;
    case DOT11_AUTH_ALGO_80211_SHARED_KEY:
        authString.assign("802.11 Shared");
        break;
    case DOT11_AUTH_ALGO_WPA:
        authString.assign("WPA");
        break;
    case DOT11_AUTH_ALGO_WPA_PSK:
        authString.assign("WPA-PSK");
        break;
    case DOT11_AUTH_ALGO_WPA_NONE:
        authString.assign("WPA-None");
        break;
    case DOT11_AUTH_ALGO_RSNA:
        authString.assign("RSNA");
        break;
    case DOT11_AUTH_ALGO_RSNA_PSK:
        authString.assign("RSNA with PSK");
        break;
    default:
        authString.assign("Other");
        break;
    }
    return authString;
}

std::string getCipherAlgorithmString(DOT11_CIPHER_ALGORITHM cipherAlgorithm) {
    std::string ciphString;
    switch (cipherAlgorithm) {
    case DOT11_CIPHER_ALGO_NONE:
        ciphString.assign("None");
        break;
    case DOT11_CIPHER_ALGO_WEP40:
        ciphString.assign("WEP-40");
        break;
    case DOT11_CIPHER_ALGO_TKIP:
        ciphString.assign("TKIP");
        break;
    case DOT11_CIPHER_ALGO_CCMP:
        ciphString.assign("CCMP");
        break;
    case DOT11_CIPHER_ALGO_WEP104:
        ciphString.assign("WEP-104");
        break;
    case DOT11_CIPHER_ALGO_WEP:
        ciphString.assign("WEP");
        break;
    default:
        ciphString.assign("Other");
        break;
    }
    return ciphString;
}

unsigned int getChForFrequency(unsigned long kHz) {
    unsigned int chFreq = 2412; //first channel centered here
    int ch = 1;
    for (ch = 1; ch <= 13 && chFreq != kHz / 1000; ch++) {
        chFreq += 5; // center step 5 mhz
    }
    return ch;
}