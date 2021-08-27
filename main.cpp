#include <iostream>
#include <curl/curl.h>

using namespace std;

struct memory {
	char *response;
	size_t size;
};

size_t WriteMem(char *data, size_t size, size_t nmemb, void *writeData) {
	size_t real_size = size * nmemb;

	struct memory *mem = (struct memory *)writeData;

  char *ptr = (char *)realloc(mem->response, mem->size + real_size + 1);
	string tmp = "helo";

  mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, real_size);
	mem->size += real_size;
	mem->response[mem->size] = 0;

	return real_size;
}

class HtmlParser {
	char *page;
	size_t pageSize;
	int indent;
	string tag;
	bool insideTag;
	bool isClosingTag;	
	bool needsNewLine;
	public:
	HtmlParser(char *_page, size_t _pageSize) {
		page = _page;
		pageSize = _pageSize;
		indent = 0;
		tag = "";
		insideTag = false;
		isClosingTag = false;
		needsNewLine = false;
	}

	void setNewLine() {
		cout << '\n';
		for (int i = 0; i < indent; i++) {
			cout << ' ';
		}
	}

	void printPage() {
		for (int i = 0; i < pageSize; i++) {
			if (page[i] == '<') {
				tag = "";
				insideTag = true;
				if (page[i + 1] == '/') 
					isClosingTag = true;
			} else if (insideTag) {
				if (page[i] == '>') {
					if (tag != "!doctype html") {
						if (isClosingTag && page[i + 1] == '<' && page[i + 2] == '/') 
							indent -= 2;
						else if (!isClosingTag && page[i - 1] != '/' && !(page[i + 1] == '<' && page[i + 2] == '/'))
							indent += 2;
					}
					insideTag = false;
					isClosingTag = false;
				} else if (!isClosingTag) 
					tag += tolower(page[i]);
				if (!insideTag){
					needsNewLine = true;
				}
			} else {
				if (page[i + 1] == '<') {
					indent -= 2;
					needsNewLine = true;
				}
			} 
			cout << page[i];
			if (needsNewLine) {
				setNewLine();
				needsNewLine = false;
			}
		}
	}
};


int main() {
	CURL *curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		struct memory buffer = {0};
		curl_easy_setopt(curl, CURLOPT_URL, "https://vercel.com/dashboard");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMem);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &buffer);
		res = curl_easy_perform(curl);
		HtmlParser *parser = new HtmlParser(buffer.response, buffer.size);
		parser->printPage();
		curl_easy_cleanup(curl);
	}
}