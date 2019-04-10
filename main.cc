
#include <iostream>
#include <fstream>

#include "libautomusic.h"

int main(int argc, char *argv[])
{
  if (libam_require_version(1,0,0))
    {
      std::cerr << "libautomusic version mismatch." << std::endl;
      return 1;
    }

  std::cout << "libam_create_context()" << std::endl;
  am_context_t *am_context = libam_create_context();
  if (!am_context)
    {
      std::cerr << "Failed to initialize the libautomusic." << std::endl;
      return 1;
    }
  std::cout << "libam_composite_by_image()" << std::endl;
  if ( int err = libam_composite_by_image(am_context, 2, 3, argv[1]) )
    {
      std::cerr << "Failed on libam_composite_by_image(): err = " << err << "." << std::endl;
      return 1;
    }
  std::cout << "libam_output_file()" << std::endl;
  if ( int err = libam_output_file(am_context, 0, "./comp.mid") )
    {
      std::cerr << "Failed on libam_output_file(): err = " << err << "." << std::endl;
      return 1;
    }
  std::cout << "libam_output_file()" << std::endl;
  if ( int err = libam_output_file(am_context, 1, "./comp.pcm") )
    {
      std::cerr << "Failed on libam_output_file(): err = " << err << "." << std::endl;
      return 1;
    }

  libam_free_context(am_context);
  return 0;
}
