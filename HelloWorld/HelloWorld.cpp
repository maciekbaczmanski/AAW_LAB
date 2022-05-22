/**********************************************************************
Copyright �2015 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

�	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
�	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

#define SUCCESS 0
#define FAILURE 1

using namespace std;

/* convert the kernel file into a string (needed for test functions) */

int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if(f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size+1];
		if(!str)
		{
			f.close();
			return 0;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}
	cout<<"Error: failed to open file\n:"<<filename<<endl;
	return FAILURE;
}



int main(int argc, char* argv[])
{

	/* Getting number of platforms platforms*/
	cl_uint numPlatforms;	
	cl_int	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		cout << "Error: Getting platforms!" << endl;
		return FAILURE;
	}

	if(numPlatforms > 0)
	{
        /* Getting all available platforms */
		cl_platform_id* platforms = (cl_platform_id* )malloc(numPlatforms* sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);


		for(int i=0;i<numPlatforms;i++)
        {

            cl_uint				numDevices = 0;
            cl_device_id        *devices;

            /* Getting number of devices available on platform */
            status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
            devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

            /* Getting all of devices available on platform */
            status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
            char platform_name[100];
            char platform_version[100];

            /* Getting platform info- name and version */
            status = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platform_name), &platform_name, NULL);
            status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(platform_version), &platform_version, NULL);
            cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout<<"                              PLATFORM\n\n";
            cout<<"Name:                    "<<platform_name<<"\n";
            cout<<"Version:                 "<<platform_version<<"\n\n";
            cout<<"-------------------------------DEVICES------------------------------\n\n";
            if (numDevices>0)
            {
                for (int j=0;j<numDevices;j++)
                {

                    char driver_version[100];
                    char device_name[100];

                    /* Getting device info- name and version */
                    status = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);
                    status = clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, sizeof(driver_version), &driver_version, NULL);
                    cout<<"Name:               "<<device_name<<"\n";
                    cout<<"OpenCL Version:     "<<driver_version<<"\n\n";

                    /* check CL_DEVICE_OPENCL_C_ALL_VERSIONS */
                    size_t num_versions =0;
                    cl_name_version *opencl_c_all_versions;
                    status = clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_ALL_VERSIONS, 0, NULL, &num_versions);
                    if (status!=0) cout<<"status:     "<<status<<"\n\n";
                    if (num_versions>0)
                    {
                        opencl_c_all_versions = (cl_name_version*)malloc(num_versions);
                        status = clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_ALL_VERSIONS, num_versions, opencl_c_all_versions, NULL);
                        for(int k=0; k<num_versions / sizeof(cl_name_version); k++)
                        {
                            cout<<"OpenCL C Version:     "<<opencl_c_all_versions[k].version<<"\n";
                        }
                        
                    }

                    

                }


                /* Create program and kernel for test purposes, basing on "HelloWorld_Kernel.cl" from ZAW excercises */

                cl_context context = clCreateContext(NULL,numDevices, devices,NULL,NULL,NULL);
                const char *filename = "HelloWorld_Kernel.cl";
                string sourceStr;
                status = convertToString(filename, sourceStr);
                const char *source = sourceStr.c_str();
                size_t sourceSize[] = {strlen(source)};
                cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
                
                /* Build program. */
                status=clBuildProgram(program, numDevices ,devices,NULL,NULL,NULL);

                /* Create kernel object */
                cl_kernel kernel = clCreateKernel(program,"helloworld", NULL);

                /* ------------------------------TESTS---------------------------------------------- */
                
                /* Testing clCreateBufferWithProperties (available only in OpenCL 3.0)*/
                try{
                cout<<"\nTesting clCreateBufferWithProperties (available only in OpenCL 3.0)\n";
                
                    cl_int errcode0 = 0;
                    
                    // cl_context context = clCreateContext(NULL,numDevices, devices,NULL,NULL,NULL);
                    cl_mem outputBuffer = clCreateBufferWithProperties(context, NULL ,CL_MEM_WRITE_ONLY , 5 * sizeof(char), NULL, &errcode0);

                    if (errcode0==0)
                    {
                        cout<<"Test PASSED!"<<"\n";
                    }
                    else
                    {
                        cout<<"Test FAILED! Error code: "<<errcode0<<"\n";
                    }
                } catch (const char* msg)
                {
                    cerr << msg << endl;
                }

                

                /* Testing CL_PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT, (available since OpenCL 2.2) */
                try{
                cout<<"\nTesting CL_PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT, (available since OpenCL 2.2)\n";
                
                    cl_int errcode1 = 0;
                
                    cl_bool value = 0;

                    errcode1 = clGetProgramInfo(program, CL_PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT, sizeof(value), &value, NULL);

                    if (errcode1==0)
                    {
                        cout<<"Test PASSED!"<<"\n";
                    }
                    else
                    {
                        cout<<"Test FAILED! Error code: "<<errcode1<<"\n";
                    }
                } catch (const char* msg)
                {
                    cerr << msg << endl;
                }



                /* Testing clCloneKernel, (available since OpenCL 2.1) */
                try{
                cout<<"\nTesting clCloneKernel, (available since OpenCL 2.1)\n";
                
                    cl_int errcode2 = 0;
                    
                    cl_kernel nwe_kernel = clCloneKernel(kernel, &errcode2);
                    if (errcode2==0)
                    {
                        cout<<"Test PASSED!"<<"\n";
                    }
                    else
                    {
                        cout<<"Test FAILED! Error code: "<<errcode2<<"\n";
                    }
                } catch (const char* msg)
                {
                    cerr << msg << endl;
                }



                /* Testing clCreatePipe, (available since OpenCL 2.0) */
                try{
                cout<<"\nTesting clCreatePipe, (available since OpenCL 2.0)\n";
                
                    cl_int errcode3 = 0;
                    
                    cl_mem Pipe = clCreatePipe(context, 0, 1, 1, NULL, &errcode3);
                    if (errcode3==0 || errcode3 == -59)
                    {
                        cout<<"Test PASSED!"<<"\n";
                    }
                    else
                    {
                        cout<<"Test FAILED! Error code: "<<errcode3<<"\n";
                    }
                } catch (const char* msg)
                {
                    cerr << msg << endl;
                }


                /* Testing clUnloadPlatformCompiler, (available since OpenCL 1.2) */
                try{
                cout<<"\nTesting clUnloadPlatformCompiler, (available since OpenCL 1.2)\n";
                
                    cl_int errcode4 = clUnloadPlatformCompiler(platforms[i]);
                    

                    // cl_mem result_image = clCreateImage(context, CL_MEM_WRITE_ONLY, &image_format, &image_desc, NULL, &errcode4);
                    if (errcode4==0)
                    {
                        cout<<"Test PASSED!"<<"\n";
                    }
                    else
                    {
                        cout<<"Test FAILED! Error code: "<<errcode4<<"\n";
                    }
                } catch (const char* msg)
                {
                    cerr << msg << endl;
                }


                /* Testing CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, (available since OpenCL 1.1) */
                try{
                cout<<"\nTesting CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, (available since OpenCL 1.1)\n";
                
                    cl_int errcode5 = 0;
                
                    cl_uint value = 0;

                    errcode5 = clGetDeviceInfo(devices[0], CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, sizeof(value), &value, NULL);

                    if (errcode5==0)
                    {
                        cout<<"Test PASSED!"<<"\n";
                    }
                    else
                    {
                        cout<<"Test FAILED! Error code: "<<errcode5<<"\n";
                    }
                } catch (const char* msg)
                {
                    cerr << msg << endl;
                }

            }

            free(devices);

        }
		free(platforms);
	}

	std::cout<<"Finished!\n";
	return SUCCESS;
}
