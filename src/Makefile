all:
	g++ main.cpp mail.cpp -O2 -I/usr/share/rapidjson/include -DRAPIDJSON_HAS_STDSTRING=1 --std=c++11 -fPIC -lfastcgi-daemon2 \
	-shared -o libMailServiceFastCGI.so && \
	cp libMailServiceFastCGI.so /app/lib
