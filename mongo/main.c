#include <bson.h>
#include <mongoc.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

unsigned int getTime()
{
	time_t tm = time(0);
	struct tm *st = localtime(&tm);
	unsigned int t = (st->tm_mon + 1) * 100000000 + st->tm_mday * 1000000
		+ st->tm_hour * 10000 + st->tm_min * 100 + st->tm_sec;
	return t;
}

void getRandNum(char* sender)
{
	// 位数
	int num = rand() % 10 + 5;

	sender[0] = '1';
	for (int i = 1; i < num; i++) {
		sender[i] = rand() % 10 + '0';
	}
}

bson_t* get_test_bson()
{
	char        buf[16];
	const       char *key;
	size_t      keylen;

	bson_t *document = bson_new();
	bson_t child, child2;
	// 生成源号码
	int64_t st = 13000000000l + rand();
	BSON_APPEND_INT64(document, "source", st);

	BSON_APPEND_ARRAY_BEGIN(document, "senders", &child);

	int sender_num = rand() % 10 + 1;
	char sender[64] = { 0 };
	for (int i = 0; i < sender_num; ++i) {
		keylen = bson_uint32_to_string(i, &key, buf, sizeof buf);

		bson_append_document_begin(&child, key, (int)keylen, &child2);

		getRandNum(sender);

		int64_t tt =  atol(sender);

		BSON_APPEND_INT64(&child2, "s", tt);
		//BSON_APPEND_UTF8(&child2, "s", sender);

		BSON_APPEND_INT32(&child2, "n", rand() % 20 + 1);
		bson_append_document_end(&child, &child2);
	}

	bson_append_array_end(document, &child);

	/*
	* print the document as a json string.
	*/
	//char       *str;
	//str = bson_as_json(document, NULL);
	//printf("%s\n", str);
	//bson_free(str);

	return document;
}

int main()
{
	srand(time(0));

	// Required to initialize libmongoc's internals 初始化libmongoc驱动
	// 结束时再调用 mongoc_cleanup();
	mongoc_init();
	mongoc_client_t *client;
	mongoc_database_t *database;
	mongoc_collection_t *collection;
	bson_error_t error;

	// Create a new client instance 创建连接对象
	client = mongoc_client_new("mongodb://10.10.10.12:27017");

	// Get a handle on the database "db_name" and collection "coll_name"
	// 获取指定数据库和集合的句柄
	database = mongoc_client_get_database(client, "test");
	collection = mongoc_client_get_collection(client, "test", "test_array");
	
	//bson_t *command, reply;
	//// Do work. This example pings the database, prints the result as JSON and performs an insert
	//command = BCON_NEW("ping", BCON_INT32(1));

	//bool retval = mongoc_client_command_simple(client, "admin", command, NULL, &reply, &error);//执行命令  
	//if (!retval) {
	//	printf("%d - %s\n", __LINE__, error.message);
	//	return EXIT_FAILURE;
	//}

	//char *str = bson_as_json(&reply, NULL);
	//printf("%d - %s\n", __LINE__, str);

	//// 释放
	//bson_free(str);
	//bson_destroy(&reply);
	//bson_destroy(command);


	int i;
	for (i = 0; i < 500; i++) {
		if (i % 10000 == 0) {
			printf("%u -- %d\n", getTime(), i);
		}

		bson_t *document = get_test_bson();

		if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, document, NULL, &error)) {
			printf("%d - %s\n", __LINE__, error.message);
		}
		bson_destroy(document);
		document = NULL;
	}

	printf("%u -- %d is ok!\n", getTime(), i);

	sleep(3);

	printf("over!\n");

	/*
	* Release our handles and clean up libmongoc
	*/
	mongoc_collection_destroy(collection);//释放表对象
	mongoc_database_destroy(database);//释放数据库对象
	mongoc_client_destroy(client);//释放连接对象
	// 释放libmongoc驱动
	mongoc_cleanup();
	return 0;
}
