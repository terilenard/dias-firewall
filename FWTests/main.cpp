/*
* This work is licensed under the terms of the MIT license.  
* For a copy, see <https://opensource.org/licenses/MIT>.
*
* Developed by NISLAB - Network and Information Security Laboratory
* at George Emil Palade University of Medicine, Pharmacy, Science and
* Technology of Târgu Mureş <https://nislab.umfst.ro/>
*
* Contributors: Bela Genge
*/

#include "../FWCoreLibrary/FWCore.h"
#include <stdio.h>

bool test0x23_permit(int fwInst);
bool test0x23_drop(int fwInst);
bool test_state_chain(int fwInst);

int main( void )
{
	int fwInst = createFWInstance("../diasfw.cfg");
	if (fwInst == FWCORE_ERROR) {
		printf("Error while creating new fw instance");
		return -1;
	}

	// Test 1:
	if (test0x23_permit(fwInst)) {
		printf("Test 1 PASSED!\n");
	}
	else {
		printf("*Test 1 FAILED!\n");
	}

	// Test 2:
	if (test0x23_drop(fwInst)) {
		printf("Test 2 PASSED!\n");
	}
	else {
		printf("*Test 2 FAILED!\n");
	}

	// Test 3: state chain
	if (test_state_chain(fwInst)) {
		printf("Test 3 (state chain) PASSED!\n");
	}
	else {
		printf("*Test 3 (state chain) FAILED!\n");
	}

	destroyFWInstance(fwInst);


	return 0;
}

bool test0x23_permit(int fwInst)
{
	int idx = 23;
	unsigned char payload[] = { 10, 8, 233, 45 };
	return (processMessage(fwInst, idx, payload, sizeof(payload)) == FWCORE_PROC_PERMIT_LOG);
}

bool test0x23_drop(int fwInst)
{
	int idx = 23;
	unsigned char payload[] = { 0, 210, 25, 0, 0, 0, 10, 2 };

	return (processMessage(fwInst, idx, payload, sizeof(payload)) == FWCORE_PROC_DROP);
}

bool test_state_chain(int fwInst)
{
	int idx1 = 25;
	unsigned char payload1[] = { 0, 210, 25, 75, 0, 0, 10, 2 };

	int idx2 = 26;
	unsigned char payload2[] = { 0, 13, 5, 75, 0, 0, 10, 2 };

	int idx3 = 27;
	unsigned char payload3[] = { 0, 17, 3, 75, 0, 0, 10, 2 };

	int idx4 = 23;
	unsigned char payload4[] = { 10, 2, 3, 75, 0, 0, 10, 2 };

	if (processMessage(fwInst, idx1, payload1, sizeof(payload1)) != FWCORE_PROC_PERMIT) {
		return false;
	}

	if (processMessage(fwInst, idx2, payload2, sizeof(payload2)) != FWCORE_PROC_PERMIT) {
		return false;
	}

	if (processMessage(fwInst, idx3, payload3, sizeof(payload3)) != FWCORE_PROC_PERMIT) {
		return false;
	}

	return (processMessage(fwInst, idx4, payload4, sizeof(payload4)) == FWCORE_PROC_DROP);
}