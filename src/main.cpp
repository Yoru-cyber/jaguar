#include <curl/curl.h>
#include <curl/easy.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
struct Source
{
  std::string name;
  std::string lang;
  std::string id;
  std::string baseUrl;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE (Source, name, lang, id, baseUrl)
struct Repo
{
  std::string name;
  std::string lang;
  int nsfw;
  std::vector<Source> sources;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE (Repo, name, lang, nsfw, sources)

json
read_json (std::string json_file_path)
{
  std::ifstream f (json_file_path);
  json data = json::parse (f);
  return data;
}
size_t
write_callback (void *contents, size_t size, size_t nmemb, std::string *userp)
{
  size_t total_size = size * nmemb;
  userp->append (static_cast<char *> (contents), total_size);
  return total_size;
}
void
call_repo_url (std::string url, CURL *curl)
{
  long http_response_code = 0;
  curl_easy_setopt (curl, CURLOPT_URL, url.c_str ());
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_getinfo (curl, CURLINFO_HTTP_CODE, &http_response_code);
  CURLcode res = curl_easy_perform (curl);
  if (res != CURLE_OK)
    {
      std::fprintf (stderr, "curl_easy_perform() failed:%s\n",
                    curl_easy_strerror (res));
    }
  else
    {
      if (http_response_code == 200)
        {
          std::cout << url << " is available" << std::endl;
        }
      else
        {
          std::cout << url << " Res code: " << http_response_code << std::endl;
        }
    }
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
      curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_callback);
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
          json data = read_json (file_name);
          auto repo_json = data.template get<std::vector<Repo>> ();
          for (auto repo : repo_json)
            {
              if (repo.lang == "es")
                {
                  call_repo_url (repo.sources[0].baseUrl, curl);
                }
            }
          curl_easy_cleanup (curl);
        }
    }
  curl_global_cleanup ();
  return 0;
}
