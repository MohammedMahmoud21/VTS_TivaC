#include "Stack.h"
#include "stdint.h"

static float stack_arr[STACK_SIZE];
static int32_t top = -1;





// implementation of push()

uint8_t push(float data)
{
    uint8_t Local_u8ErrorState = 0;

    if (top == STACK_SIZE-1)
    {
        Local_u8ErrorState = 1;
    }
    else
    {
        top += 1;
        stack_arr[top] = data;
    }

    return (Local_u8ErrorState);
}


// implementation of pop()

uint8_t pop(float *Copy_pf32data)
{
    uint8_t Local_u8ErrorState = 0;
    if (top == -1)
    {
        Local_u8ErrorState = 1; /*1 == Empty*/
    }
    else
    {
        *Copy_pf32data = stack_arr[top];
        top -= 1;
    }
    return (Local_u8ErrorState);
}

uint8_t Read_u8Top(float *Copy_pf32data)
{
    uint8_t Local_u8ErrorState = 0;

    if((void *)0 == Copy_pf32data)
    {
        Local_u8ErrorState = 1;
    }
    else if (top == -1)
    {
        Local_u8ErrorState = 2; /*2 == Empty*/
    }
    else
    {
        *Copy_pf32data = stack_arr[top];
    }

    return (Local_u8ErrorState);
}
