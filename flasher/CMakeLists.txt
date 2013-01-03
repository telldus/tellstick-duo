PROJECT(tellstickduo-flasher)

SET(SRCS
  main.cpp
)

ADD_EXECUTABLE(tellstickduo-flasher
  ${SRCS}
)

FIND_LIBRARY(FTD2XX_LIBRARY ftd2xx)

TARGET_LINK_LIBRARIES(tellstickduo-flasher
	${FTD2XX_LIBRARY}
)

