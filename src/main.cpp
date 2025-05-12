#include "cstdio"
#include "curl/curl.h"
#include "curl/easy.h"

int
main (int argc, char *argv[])
{
  CURL *curl;
  curl_global_init (CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init ();
  if (curl)
    {
      CURLcode res;
      curl_easy_setopt (curl, CURLOPT_URL, "https://google.com");
      res = curl_easy_perform (curl);
      if (res != CURLE_OK)
        {
          std::fprintf (stderr, "curl_easy_perform() failed:%s\n",
                        curl_easy_strerror (res));
        }
      curl_easy_cleanup (curl);
    }
  curl_global_cleanup ();
  return 0;
}
