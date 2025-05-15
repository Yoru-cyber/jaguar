#include "curl/curl.h"
#include "curl/easy.h"
#include <fstream>
#include <iostream>
size_t
write_data (void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t written = fwrite (ptr, size, nmemb, stream);
  return written;
}
size_t
WriteCallback (void *contents, size_t size, size_t nmemb, std::string *userp)
{
  size_t total_size = size * nmemb;
  userp->append (static_cast<char *> (contents), total_size);
  return total_size;
}
int
main (int argc, char *argv[])
{
  CURL *curl;
  curl_global_init (CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init ();
  std::string file_url = "https://raw.githubusercontent.com/keiyoushi/"
                         "extensions/refs/heads/repo/index.min.json";
  std::string file_name = "domains.json";
  std::string response;
  if (curl)
    {
      CURLcode res;
      curl_easy_setopt (curl, CURLOPT_URL, file_url.c_str ());
      curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response);
      res = curl_easy_perform (curl);
      if (res != CURLE_OK)
        {
          std::fprintf (stderr, "curl_easy_perform() failed:%s\n",
                        curl_easy_strerror (res));
        }
      else
        {
          std::ofstream domains_file (file_name, std::ios::binary);
          if (!domains_file)
            {
              std::cerr << "Failed to open output file\n";
              curl_easy_cleanup (curl);
              return 1;
            }
          domains_file << response;
          domains_file.close ();
          curl_easy_cleanup (curl);
        }
    }
  curl_global_cleanup ();
  return 0;
}
