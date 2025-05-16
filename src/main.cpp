#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <list>
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
struct RepoJson
{
  std::string name;
  std::string lang;
  int nsfw;
  std::vector<Source> sources;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE (RepoJson, name, lang, nsfw, sources)

json
read_json (std::string json_file_path)
{
  std::ifstream f (json_file_path);
  json data = json::parse (f);
  return data;
}
void
callRepoUrl (std::string url)
{
  std::cout << url << std::endl;
}
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
          json data = read_json (file_name);
          auto repo_json = data.template get<std::vector<RepoJson>> ();
          for (auto repo : repo_json)
            {
              if (repo.lang == "es")
                {
                  callRepoUrl (repo.sources[0].baseUrl);
                }
            }
          curl_easy_cleanup (curl);
        }
    }
  curl_global_cleanup ();
  return 0;
}
