#include <string>
#include "CipherInterface.h"
#include "DES.h"
#include "AES.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <string>

using namespace std;

// gets file size
size_t getFilesize(FILE* filename)
{
    struct stat st;
    int fp = fileno(filename);
    if(fstat(fp, &st) != 0)
		{
        return 0;
    }

    return st.st_size;
}

int main(int argc, char** argv)
{
	/**
	 * TODO: Replace the code below	with your code which can SWITCH
	 * between DES and AES and encrypt files. DO NOT FORGET TO PAD
	 * THE LAST BLOCK IF NECESSARY.
	 *
	 * NOTE: due to the incomplete skeleton, the code may crash or
	 * misbehave.
	 */
    /* Create an instance of the DES cipher */
    CipherInterface* cipher = NULL;
		const unsigned char *cipherText = NULL;
		const unsigned char *plainText = NULL;
    const unsigned char *key = NULL;
    unsigned char key2[33];

    FILE* in_file;
    FILE* out_file;
    string aes_or_dec = argv[1];
    key = (const unsigned char *)argv[2];
    string enc_or_dec = argv[3];
    char* input = argv[4];
    char* output = argv[5];


    /* Set the encryption key
     * A valid key comprises 16 hexidecimal
     * characters. Below is one example.
     * Your program should take input from
     * command line.
     */

    //cipher->setKey((unsigned char*)"0123456789abcdef");
    if(aes_or_dec == "DES")
		{
        cipher = new DES();
        cout << "DES IS PICKED, SETTING KEY NOW" << endl;
        cipher->setKey(key);
    }
		else if(aes_or_dec == "AES")
		{
        cipher = new AES();
        cout << "AES IS PICKED, SETTING KEY NOW" << endl;
            strncpy((char*)key2+1, (const char*)key, 33);
    }
		else cipher = NULL;

    /* Error checks */
    if(!cipher)
    {
        fprintf(stderr, "ERROR [%s %s %d]: could not allocate memory\n",
                __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }

    in_file = fopen(input, "rb");
    out_file = fopen(output, "wb");

		// checks if file is open or not
    if(!in_file)
		{
        perror("fopen");
        exit(-1);
    }
    if(!out_file)
		{
        perror("fopen");
        exit(-1);
    }
    size_t fsize = getFilesize(in_file);

    size_t counter = 1;

    long end;
    size_t result;

    if(in_file!=NULL)
		{
        //getting the 8 or 16 character string
        if(aes_or_dec == "DES")
				{
            end = 8;
        }
				else if(aes_or_dec == "AES")
				{
            end = 16;
            if(enc_or_dec == "ENC")
						{
                key2[0] = '0';
            }
						else
						{
                key2[0] = '1';
            }
            cipher->setKey(key2);
        }
    // gets number of blocks
    size_t num_blocks = fsize/end;

    // partial blocks
    if(fsize % end > 0)
	    ++num_blocks;

    // # of padding
    size_t padding = end - (fsize%end);

    // stores second to last block of plaintext
    const unsigned char *tempPlainText;
    tempPlainText = new unsigned char[fsize];

    // buffer to store input from in_file
    unsigned char buffer[end+1];

        while(!feof(in_file))
				{
            memset(buffer, 0, end+1);
            result = fread(buffer, 1, end, in_file);

            // file is at end when result !=0
            if(result != 0)
						{
                if(result != end && enc_or_dec != "DEC")
								{
                    // pads if necessary
                    for(long int i = result; i < end; i++)
										{
                        buffer[i] = '0';
                    }
                }

								cout << "LINE CHUNK: " << buffer << endl;
                if(enc_or_dec == "ENC")
								{
                    cipherText = cipher->encrypt((const unsigned char*)buffer);
                    cout << "STARTING ENCRYPTION \n";
                    // encrypts up to 8 character string
                    fwrite(cipherText, sizeof(char), end, out_file);
                    if (counter == num_blocks)
										{
												// clear buffer
                        memset(buffer,0,end+1);
                        for(long int i = 0; i < end; i++)
												{
                            if(i == end-1)
														{
                                // store the padding in the last index.
                                buffer[i] = padding;
                            }
														else
														{
                                buffer[i] = '0';
                            }
                        }

                        cipherText = cipher->encrypt((const unsigned char*)buffer);
                        fwrite(cipherText, sizeof(char), end, out_file);
                    }

                    delete[] cipherText;
                }
								else if(enc_or_dec == "DEC")
								{
                    plainText = cipher->decrypt((const unsigned char*)buffer);
                    cout << "STARTING DECRYPTION \n";

                    if (counter == num_blocks-1)
										{
                        strcat((char*)tempPlainText, (char*)plainText);
                    }
										else if(counter==num_blocks)
										{
                        int rmPadding = plainText[end-1];
                        strcat((char*)tempPlainText, (char*)plainText);

                        // write to the file, while removing padding
                        fwrite(tempPlainText, sizeof(char), end-rmPadding, out_file);
                        delete[] tempPlainText;
                    }
										else if(counter!=num_blocks)
										{
                        fwrite(plainText, sizeof(char), end, out_file);
                    }

                    delete[] plainText;
                }

                counter++;
            }
        }

        fclose(in_file);
        fclose(out_file);
    }
		else cout << "Unable to open file";

    cout << "Finished\n";

	return 0;
}
