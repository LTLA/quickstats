include(FetchContent)

FetchContent_Declare(
  sanisizer
  GIT_REPOSITORY https://github.com/LTLA/sanisizer
  GIT_TAG master # ^0.2.0
)

FetchContent_Declare(
  auveh 
  GIT_REPOSITORY https://github.com/LTLA/auveh
  GIT_TAG master # ^0.1.0
)

FetchContent_MakeAvailable(sanisizer)
FetchContent_MakeAvailable(auveh)
