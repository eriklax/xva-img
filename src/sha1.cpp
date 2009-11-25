#include <openssl/sha.h>
#include <stdio.h>
#include "sha1.hpp"

std::string XVA::SHA1(const std::string& input)
{
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	SHA1_Update(&ctx, input.c_str(), input.size());

	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Final(hash, &ctx);

	std::string result;
	char hex[3];
	for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		snprintf(hex, sizeof(hex), "%02x", hash[i]);
		result.append(hex, 2);
	}
	return result;
}
