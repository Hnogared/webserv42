#!/bin/bash

cat test/requests/post_test.txt

curl -X POST -v --data-binary @test/requests/post_test.txt http://mywebsite.com:9090/test
