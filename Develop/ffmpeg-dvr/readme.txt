1 、0.50版本代码位置在

     \\192.168.1.3\Public\project\iPhone\ffmpge

2、configure 参数

./configure --prefix=/Developer/usr/ --disable-debug --cc=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/arm-apple-darwin9-gcc-4.2.1 --as='gas-preprocessor.pl /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/arm-apple-darwin9-gcc-4.2.1' --sysroot=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.sdk/ --enable-cross-compile --target-os=darwin --arch=arm --cpu=arm1176jzf-s  --enable-static  --enable-swscale_alpha --disable-debug --disable-zlib --disable-bzlib --disable-ffmpeg --disable-encoders --disable-hwaccels --disable-muxers --disable-demuxers --disable-protocols --disable-filters --disable-bsfs --disable-decoders --enable-decoder=mpeg4 --disable-armv5te --disable-armv6 --enable-decoder=aac  --enable-version3