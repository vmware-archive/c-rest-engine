/**************************************************************************
* This test client uses libcurl to open connection to server and re-uses
* the same connection to send multiple HTTPS requests.
**************************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>


struct string
{
    char                            *ptr;
    size_t                          len;
};

void init_string(struct string *s)
{
      s->len = 0;
      s->ptr = malloc(s->len+1);
      if (s->ptr == NULL)
      {
          fprintf(stderr, "malloc() failed\n");
          exit(EXIT_FAILURE);
      }
      s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t                           new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL)
    {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size*nmemb;
}

int main (int argc, char *argv[])
{
    CURL*                            curl = NULL;
    CURLcode                         res = 0;
    int                              ct = 0;
    char                             expected[512] = {0};

    if (argc != 2) {
        printf("Wrong number of arguments supplied to test program\n");
        exit(1);
    }

    struct string s;
    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(curl)
    {
try:
        init_string(&s);
        struct curl_slist *chunk = NULL;

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        if (ct == 500)
        {
            chunk = curl_slist_append(chunk, "Connection:close");
            strcpy(expected,"HTTP/1.1 200 OK\r\nConnection:close\r\nContent-Length:13\r\n\r\nKumar Kaushik");
        }
        else
        {
            chunk = curl_slist_append(chunk, "Connection:keep-alive");
            strcpy(expected,"HTTP/1.1 200 OK\r\nConnection:keep-alive\r\nContent-Length:13\r\n\r\nKumar Kaushik");
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "Kumar Kaushik");
        curl_easy_setopt(curl, CURLOPT_URL, argv[1]);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        if (strcmp(expected, s.ptr) == 0)
        {
            printf("Test passed, iteration %d\n", ct);
        }
        else
        {
           printf("Test failed, iteration %d\n", ct);
        }
        //printf("\n===============\n%s\n==========\n", s.ptr);
        free(s.ptr);

        ct++;

        if (ct <= 500)
        {
            goto try;
        }

        curl_easy_cleanup(curl);
    }

  return 0;
}

