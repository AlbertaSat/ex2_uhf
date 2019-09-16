
/******************************************************************************************************************
	The following are placeholders. Replace anything (parameter types, return type, etc) as required.
******************************************************************************************************************/

#define STUB_CORRECTION_CODE	0xbaddc0de

// Returns forward error correction data for a given message.
int ComputeFwdErrCorrection( char *pMessage, int nMessageSize )
{
	// Stub...
	return STUB_CORRECTION_CODE;
}

// DESTRUCTIVE - overwrites pMessage in place.
// Given a message and correction data (as obtained by ComputeFwdErrCorrection()),
// this verifies and attempts to correct any errors in the message.
// Returns number of errors corrected, or -1 in case of failure.
int ApplyErrCorrection( char *pMessage, int nMessageSize, int nCorrectionData )
{
	// Stub...
	if (nCorrectionData == STUB_CORRECTION_CODE)
	{
		return 0;
	}

	// Failure
	return -1;
}
