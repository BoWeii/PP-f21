#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //

  __pp_vec_float clamp_float = _pp_vset_float(9.999999f);
  __pp_vec_int zero_int = _pp_vset_int(0);
  __pp_vec_int one_int = _pp_vset_int(1);

  __pp_mask maskElse, maskZero, maskWork, maskClamp;

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    __pp_vec_int y, count;
    __pp_vec_float x;
    __pp_vec_float result = _pp_vset_float(0.f);
    int work_range = N - i >= VECTOR_WIDTH ? VECTOR_WIDTH : N - i;

    __pp_mask maskIf = maskZero = _pp_init_ones(0);
    __pp_mask maskWork = _pp_init_ones(work_range); //let the mask don't out of work's range

    int cntBits = 0;

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskWork);  // float x = values[i];
    _pp_vload_int(y, exponents + i, maskWork); // int y = exponents[i];

    // Set mask according to predicate
    _pp_veq_int(maskIf, y, zero_int, maskWork); // if (y == 0) {
    __pp_mask maskAval = _pp_mask_and(maskIf, maskWork);

    // Execute instruction using mask ("if" clause)
    _pp_vset_float(result, 1.f, maskAval); //   output[i] = 1.f;

    // Inverse maskIsNegative to generate "else" mask
    maskElse = _pp_mask_not(maskIf); // } else {
    maskAval = _pp_mask_and(maskElse, maskWork);

    _pp_vload_float(result, values + i, maskAval); // float result = x;
    _pp_vsub_int(count, y, one_int, maskAval);     //   int count = y - 1;
    _pp_vgt_int(maskZero, count, zero_int, maskAval);
    maskAval = maskZero;
    while (_pp_cntbits(maskAval) != 0)
    {
      _pp_vmult_float(result, result, x, maskAval);  // result *= x;
      _pp_vsub_int(count, count, one_int, maskAval); //   int count -- ;
      _pp_vgt_int(maskAval, count, zero_int, maskAval);
    }

    // Set mask according to predicate
    _pp_vgt_float(maskClamp, result, clamp_float, maskWork); // if (result > 9.999999f) {
    _pp_vmove_float(result, clamp_float, maskClamp);         //result=9.99999f

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskWork);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //
  float sum = 0;
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    __pp_mask maskAll = _pp_init_ones(VECTOR_WIDTH);
    __pp_vec_float data;
    _pp_vload_float(data, values + i, maskAll);
    int temp = VECTOR_WIDTH;
    while (temp != 1)
    {
      _pp_hadd_float(data, data);
      _pp_interleave_float(data, data);
      temp /= 2;
    }
    sum += data.value[0];
  }

  return sum;
}