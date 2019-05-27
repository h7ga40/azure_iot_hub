# Azure IoT Hub with TOPPERS/ASP3

RTOSの[TOPPERS/ASP3](https://www.toppers.jp/asp3-kernel.html)を使ったAzure IoT Hub へ接続する[Azure IoT Hub Device C SDK](https://github.com/Azure/azure-iot-sdk-c)を使ったサンプルです。
ターゲットデバイスは、[GR-SAKURA](http://gadget.renesas.com/ja/product/sakura.html)で、EthernetでHTTP通信します。
MQTTのコードも含んでいますが、現在のバージョンでは接続できていません。

[GR-PEACH](http://gadget.renesas.com/ja/product/peach.html)でも動作確認できています。ベースのソフトに[TOPPERS/ASP3版ECNL](http://dev.toppers.jp/trac_user/contrib/wiki/asp3_tinet_ecnl_rx)を使用します。注意点として、「get_time」という関数が「c-utility/adapters/agenttime_mbed.c」に定義されていますが、ベースのソフトにも定義されているので、Azure IoT SDK側の「get_time」関数を「azure_get_time」などにリネームして競合を避ける必要があります。

サンプルを動作させるには、[この手順](https://docs.microsoft.com/ja-jp/azure/iot-hub/quickstart-send-telemetry-c)や[この手順](https://github.com/ms-iotkithol-jp/IoTKitHoLV4)を参照にAzure側にデバイスを追加し、そのデバイスの接続文字列を取得し、ソースコード「app_iothub_client/src/client.c」の以下の個所を書き換える必要があります。

```c
static const char* connectionString = "[device connection string]";
```

## 開発環境

ビルドはGCC+Makefileで行います。RX向けGCCは[こちら](https://gcc-renesas.com/ja/rx-download-toolchains/)から入手できます。
IDEとして[e² studio V7.4.0](https://www.renesas.com/ja-jp/products/software-tools/tools/ide/e2studio.html)を使用しました。
Rubyの実行環境が必要なので、​[こちら](https://www.ruby-lang.org/ja/downloads/)からダウンロードして、コマンドプロントから実行できるよう、環境変数でパスを通してください。
e² studioのプロジェクトファイルも含まれているので、展開したフォルダをワークスペースとして開き、プロジェクトのインポートをこのフォルダに対して行うことで、取り込むことができます。
開発手順は[ここ](https://dev.toppers.jp/trac_user/contrib/wiki/azure_iot_hub)にあります。

## フォルダ構成

|パス|内容|ライセンス|
|:---:|:---:|---|
|app_iothub_client|サンプルプログラム|TOPPERS License|
|asp3_dcre|RTOS [TOPPERS/ASP3](http://www.toppers.jp/asp3-kernel.html)|TOPPERS License|
|asp3_dcre/tinet|TCPプロトコルスタック [TINET 1.7](http://www.toppers.jp/tinet.html)|TOPPERS License|
|asp3_dcre/syssvc/tlsf|メモリアロケータ [tlsf](https://github.com/mattconte/tlsf)|BSD License|
|azure_iothub|[Azure IoT Hub Device C SDK](https://github.com/Azure/azure-iot-sdk-c) (tag 2019-04-11)|MIT License|
|azure_iothub/deps/parson|jsonパーサー [parson](https://github.com/kgabis/parson)|MIT License|
|azure_iothub/c-utility/adapters|TOPPERS向けの実装を追加|TOPPERS License|
|curl-7.57.0|HTTPクライアント[cURL](https://curl.haxx.se/)|Curl License|
|musl-1.1.18|標準Cライブラリ[musl](http://www.musl-libc.org/)|MIT License|
|ntshell/fatfs|FATファイルシステム [FatFs](http://elm-chan.org/fsw/ff/00index_e.html)|FatFs License|
|ntshell/ntshell|仮想ターミナル [NT-Shell](https://www.cubeatsystems.com/ntshell/)|MIT  License etc.|
|ntshell/src|Socket API、標準入出力、ファイルディスクリプタ|TOPPERS License|
|wolfssl-3.15.7|SSL/TLS ライブラリ [wolfSSL](https://www.wolfssl.com/)|GPL/商用|
|zlib-1.2.11|圧縮・伸張ライブラリ [zlib](https://www.zlib.net/)|zlib License|

ライセンスは参考のため載せておきました。正確なライセンスは各ソフトウェアのサイトを確認してください。

|確認|プロトコル|ソフトスタック|
|-|-|-|
|:heavy_check_mark:|HTTP|iothubtransporthttp.c→httpapi_compact.c→tlsio_wolfssl.c→socketio_berkeley.c|
|:heavy_check_mark:|HTTP|iothubtransporthttp.c→httpapi_curl.c→Curl＋WolfSSL|
|:heavy_multiplication_x:|MQTT|iothubtransportmqtt.c→wsio.c→umqtt→socketio_berkeley.c|
|:heavy_multiplication_x:|MQTT over WebSocket|iothubtransportmqtt_websockets.c→wsio.c→umqtt→socketio_berkeley.c|
|:heavy_minus_sign:|AMQP||
|:heavy_minus_sign:|AMQP over WebSocket||

ソースコードは、一番上の「httpapi_compact.c」が有効になっています。

## TOPPERS License

TOPPERS Licenseは、BSDライクなライセンスです。詳しくは、下記のサイトを見てください。

https://www.toppers.jp/license.html
