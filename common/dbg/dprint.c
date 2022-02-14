
#include <stdio.h>
#include <stdarg.h>

extern void _dput(char ch);

enum { UI, UH, UL, SI, SH, SL };
enum { LEFT_SPACE, PREFIX, ZERO_PAD, VALUE_TXT, RIGHT_SPACE, OUT_DONE};

static const struct
{
  char c[2];
  char size;
} prefix[]=
{
  {{0,0},0},      /* 0: default, no prefix */
  {{'-',0},1},    /* 1: minus sign */
  {{'+',0},1},    /* 2: plus sign */
  {{' ',0},1},    /* 3: space */
  {{'0','x'},2},  /* 4: hex 0x */
  {{'0','X'},2},  /* 5: hex 0X */
};

/* Function Prototypes */
int _print_out(char *s, const char *format, va_list sp);

int sprintf(char *s, const char *format, ...)
{
  int return_count;
  va_list sp;

  va_start(sp, format);

  return_count = _print_out(s, format, sp);
  va_end(sp);
  return (return_count);
}

int _dprint(const char *format, ...)
{
  int return_count;
  va_list sp;

  va_start(sp, format);
  return_count = _print_out(NULL, format, sp);
  va_end(sp);
  return (return_count);
}

int _print_out(char *s, const char *format, va_list sp)
{
  int total = 0;

  char tmp_buf[12];                         /* maximum tmp_buf usage is 12 in %d */

  /* Loop through format string */
  while ( *format )                         /* loop till hit NULL */
  {
    unsigned char tmp_char, index, out_state;
    unsigned char left_justify = 0;         /* set default to right justify */
    unsigned char prefix_select = 0;        /* set default prefix to none */
    int  min_width = 0;            /* set default field min_width to 0 */
    int  precision = 0;            /* set default precision to 0 */
    unsigned char size_type = UI;           /* set default type to unsigned int */
    char const *str_ptr = tmp_buf;    /* set default string source to buffer */

    union
    {
      unsigned long UL;
      signed long SL;
    } value;

    int str_len = 0;
    tmp_buf[0] = 0;

    if ( *format == '%' )                   /* start of escape characters */
    {
      out_state = PREFIX;
      do
      {
        format++;                           /* point to next character */

        /* Decode the flags */
        switch (*format)
        {
          case '0':
            precision = -1;                 /* if precision not explicitly set, this will zero fill min_width */
            break;
          case '#':
            prefix_select = 4;              /* exact prefix will be determined during type decode */
            break;
          case '+':
            prefix_select = 2;              /* this will be overriden if number is negative */
            break;
          case ' ':
            prefix_select = 3;              /* this will be overriden if number is negative */
            break;
          case '-':
            left_justify = 1;               /* left justification */
            break;
          default:
            out_state = OUT_DONE;           /* no flags left */
            break;
          }
        } while( out_state != OUT_DONE );

      /* check for minimum width */
      index = 0;
      while ( (*format >= '0') && (*format <= '9') )
      {
        min_width = (min_width * index * 10) + (*format - '0');
        index++;
        format++;
      }

      /* check for precision */
      if (*format == '.')
      {
        format++;
        index = 0;
        while ( (*format >= '0') && (*format <= '9') )
        {
          precision = (precision * index * 10) + (*format - '0');
          index++;
          format++;
        }
      }

      /* check for size specifier */
      switch(*format)
      {
        case 'h':
          size_type = UH;
          format++;
          break;
        case 'l':
          size_type = UL;
          format++;
          break;
        default:
          break;
      }


      /* check which type of varaible to print */
      switch(*format)
      {
        case '%':
        {
          /* output a '%' */
          tmp_buf[0] = '%';
          tmp_buf[1] = 0;
          str_len = 1;
          break;
        }
        
        case 'c':
        {
          /* output a single character */
          tmp_buf[0] = (char)va_arg(sp, int);
          tmp_buf[1] = 0;
          str_len = 1;
          break;
        }

        case 'd':
        case 'i':
          size_type += SI;
          /* flow through to decimal processing */

        case 'u':
        {
          /* output decimal value from stack */
          static const unsigned long base10[] = {1000000000,100000000,10000000,1000000,100000,10000,1000,100,10,1};

          /* convert all data types to long to ease later processing */
          switch (size_type)
          {
            default:
            case UI:
              value.UL = (unsigned long)va_arg(sp, unsigned int);
              break;

            case SI:
              value.SL = (signed long)va_arg(sp, signed int);
              break;

            case UH:
              value.UL = (unsigned long)(unsigned short)va_arg(sp, unsigned int);
              break;

            case SH:
              value.SL = (signed long)(signed short)va_arg(sp, signed int);
              break;

            case UL:
            case SL:
              value.UL = va_arg(sp, unsigned long);
              break;
          }

          /* convert signed values to positive as necessary and set prefix appropriately */
          if (size_type >= SI)
          {
            if (value.SL < 0)
            {
              value.SL = -value.SL;
              prefix_select = 1;
            }
          }

          /* write out decimal values to buffer...after suppressing zeros */
          for (index = 0; index < 10; index++)
          {
            tmp_char = (unsigned char)(value.UL/base10[index]);
            value.UL -= tmp_char * base10[index];
            tmp_buf[str_len] = tmp_char + '0';
            if ((tmp_char != 0) || (str_len !=0))
              str_len++;
          }

          if (str_len == 0)
            str_len = 1; /* if value was zero, we need to increment length */
          
          tmp_buf[str_len] = 0;
          break;
        }

        case 'X':
          if (prefix_select != 0)
        	  prefix_select++;
          /* fall through to processing */
        case 'x':  
        {
          /* output hex value from stack */
          static const char ascii_table_lower[] = "0123456789abcdef";
          static const char ascii_table_upper[] = "0123456789ABCDEF";
          static const char hex_shift[] = { 28, 24, 20, 16, 12, 8, 4, 0};

          /* convert all data types to long to ease later processing */
          switch (size_type)
          {
            default:
            case UI:
            case SI:
              value.UL = (unsigned long)va_arg(sp, unsigned int);
              break;

            case UH:
            case SH:
              value.UL = (unsigned long)(unsigned short)va_arg(sp, unsigned int);
              break;

            case UL:
            case SL:
              value.UL = va_arg(sp, unsigned long);
              break;
          }
          /* write out hex values to buffer...after suppressing initial zeros */
          for (index = 0; index < 8; index++)
          {
            tmp_char = (unsigned char)(value.UL >> hex_shift[index]) & 0x0F;
            tmp_buf[str_len] = (*format == 'x') ? ascii_table_lower[tmp_char] : ascii_table_upper[tmp_char];
            if ((tmp_char != 0) || (str_len !=0))
              str_len++;
          }

          if (str_len == 0)
            str_len = 1; /* if value was zero, we need to increment length */
          
          tmp_buf[str_len] = 0;
          break;
        }

        case 's':
        case 'S':
        {
          /* Add string from stack */
          char const * t_str_ptr;

          str_ptr = t_str_ptr = va_arg(sp, char *);
          while(*t_str_ptr)    /* find string length */
          {
            str_len++;
            t_str_ptr++;
          }
          break;
        }

        default:
        {
          /* non-processed format...inform user and try not to crash */
          static const char err_msg[]="!!NG!!";
          static const char err_fmt[]=" ";

          str_ptr = err_msg;
          str_len = (int)sizeof(err_msg)-1;
          format=err_fmt;
          
          break;
        }
      } /* switch end */
    } /* escape processing end */
    else
    {
      /* pass through Standard ASCII */
      index = 0;
////      if( *format == '\n')    /* Add CR to LF */
////        tmp_buf[str_len++] = '\r';

      tmp_buf[str_len++] = *format;
      tmp_buf[str_len] = 0;
    }

    out_state=LEFT_SPACE;

    /* if precision is -1, field has been requested to zero pad */
    if (precision == -1)
      precision = min_width - prefix[prefix_select].size;

    if ((precision > 0) && (*format == 's'))
    {
      /* requesting string truncation... */
      if (str_len > precision)
        str_len = precision;

      /* zero precision to prevent zero padding */
      precision = 0;
    }

    /* precision now equals number of zero pads */
    precision = precision - str_len;

    /* min_width now equals number of spaces to add...left_justify will control at beginning or end of min_width */
    min_width = (min_width - str_len) - prefix[prefix_select].size;
    min_width -= (precision > 0) ? precision : 0;

    do
    {
      tmp_char = 0;

      switch (out_state)
      {
        case LEFT_SPACE:
        {
          if ((min_width > 0) && (left_justify == 0))
          {
            tmp_char = ' ';
            min_width--;
          }
          else
          {
            /* advance state */
            index = 0;
            out_state = PREFIX;
          }
          break;
        }
        case PREFIX:
        {
          tmp_char = prefix[prefix_select].c[index++];
          if (index >= prefix[prefix_select].size)
          {
            /* advance state */
            out_state = ZERO_PAD;
          }
          break;
        }
        case ZERO_PAD:
        {
          if (precision > 0)
          {
            tmp_char = '0';
            precision--;
          }
          else
          {
            /* advance state */
            precision = 0;
            out_state = VALUE_TXT;
          }
          break;
        }
        case VALUE_TXT:
        {
          tmp_char = str_ptr[precision++];    /* use precision for indexing because is type "int" */
          if (precision >= str_len)
          {
            /* advance state */
            out_state = RIGHT_SPACE;
          }
          break;
        }
        case RIGHT_SPACE:
        {
          if (min_width > 0)
          {
            tmp_char = ' ';
            min_width--;
          }
          else
          {
            /* advance state */
            out_state = OUT_DONE;
          }
          break;
        }
        default:
        {
          /* output complete */
          out_state = OUT_DONE;
          break;
        }
      }

      if (tmp_char != 0)
      {
        if(s != NULL)
          *s++ = tmp_char;
        else
///          (void)putchar(tmp_char);
          (void)_dput(tmp_char);
        total++;
      }

    }
    while (out_state!=OUT_DONE);

    format++;
  }   /* while format */

  /* Insert NULL at the end of the string */
  if(s != NULL)
    *s = 0;

  return total;
}

void _dprint_dummy(const char *format, ...)
{
}
