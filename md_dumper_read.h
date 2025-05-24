int Read_ROM_Auto(void)
{
    printf("\n");
    printf("Read Mode : Read ROM in automatic mode\n");
    timer_start();

    // First Search if game is in special csv gamelist

    // Search checksum cartridge in Custom Hardware games csv table
    i=0;
    for (i = 0; i < chksm_text_values_count ; i++)
    {
        strncpy(txt_csv_chksm,chksm_text_values[i],4);
        //printf(" txt chksm value : %s \n",txt_csv_chksm);
        csv_chksm = (unsigned short)strtol(txt_csv_chksm, NULL, 16);

        if ( checksum_header == csv_chksm  )
        {
            Index_chksm = i;
            printf("\n");
            printf("Found game in extra CSV Gamelist  \n");
            //printf("Position in csv table %d \n",i);
            strncpy(txt_csv_game_size,chksm_text_values[i]+5,4);
            txt_csv_game_size[4] = '\0'; // Null-terminate the output string
            //printf(" txt game size : %s \n",txt_csv_game_size);
            csv_game_size = (unsigned char)strtol(txt_csv_game_size, NULL, 10);
            //printf(" CSV Game Size  %d \n",csv_game_size);
            game_size=1024*csv_game_size;
            //printf("ROM Size from CSV is %ld Ko \n",game_size);

            //Return Hardware type
            strncpy(txt_mapper_number,chksm_text_values[i]+10,2);
            txt_mapper_number[1] = '\0'; // Null-terminate the output string
            //printf(" CSV Mapper Type  %s \n",txt_mapper_number);
            Hardwaretype = (unsigned char)strtol(txt_mapper_number, NULL, 10);
            //printf(" Hardware type  %d \n",Hardwaretype);
        }
    }

    if ( sms_mode == 0 && Hardwaretype == 0 ) // Dump Megadrive cartridge in no mapper mode
    {
        printf("Rom Size : %ld Ko \n",game_size);
        game_size = game_size*1024;
        BufferROM = (unsigned char*)malloc(game_size);
        // Cleaning ROM Buffer
        for (i=0; i<game_size; i++)
            BufferROM[i]=0x00;

        address=0;
        usb_buffer_out[0] = READ_MD;
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=1;

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
        printf("ROM dump in progress...\n");
        res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes, 0);
        if (res != 0)
        {
            printf("Error \n");
            return 1;
        }
        printf("\n");
        printf("Dump ROM completed !\n");
        timer_end();
        timer_show();
        myfile = fopen("dump_smd.bin","wb");
        fwrite(BufferROM, 1,game_size, myfile);
        fclose(myfile);
    }
    else if ( sms_mode == 0 && Hardwaretype == 2 ) // Automatic Dump Megadrive cartridge in SSF2 mapper mode
    {
        printf("Extra Hardware detected dump in mode : Sega SSF \n");
        NumberOfBank = game_size/512;
        //printf("Game Size is %ld Ko \n",game_size);
        printf("Number of Banks is %d \n",NumberOfBank);
        printf("Bank Size is 512 Ko  \n");

        game_size = game_size * 1024;
        BufferROM = (unsigned char*)malloc(game_size);
        // Cleaning ROM Buffer
        for (i=0; i<game_size; i++)
            BufferROM[i]=0x00;

        // Dump the first 4MB of the ROM as fast as possible
        printf("Bankswith bank O-7 to $080000 - $3FFFFF \n");

        printf("Dumping please wait ...\n");
        timer_start();
        address = 0;
        i=0;

        usb_buffer_out[0] = READ_MD;
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=1;

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
        printf("ROM dump in progress...\n");
        res = libusb_bulk_transfer(handle, 0x82,BufferROM,4096*1024, &numBytes,0);
        if (res != 0)
        {
            printf("Error \n");
            return 1;
        }

        ActualBank = 8;
        offset = 4096 - 1024;

        while ( offset != (game_size/1024)-1024)
        {
            printf("Bankswith bank %d - %d to $200000 - $2FFFFF \n",ActualBank,ActualBank+1);
            printf("Dumping please wait ...\n");

            address = 0xA130F9/2; // bank 4
            usb_buffer_out[0] = MAPPER_SSF2;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0;
            usb_buffer_out[5]=ActualBank;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);

            // Send 0x09 to the bank 7
            address = 0xA130FB/2; // bank 5
            usb_buffer_out[0] = MAPPER_SSF2;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0;
            usb_buffer_out[5]=ActualBank+1;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            address = (2048*1024)/2;
            // Dump lastMB
            // Do a simple read maybe needed for init bank or slow down the dumper :D
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
            libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 0);

            // Re-start the dump
            address = (2048*1024)/2;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1;

            offset = offset + 1024;
            ActualBank = ActualBank +2;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            res = libusb_bulk_transfer(handle, 0x82,BufferROM+offset*1024,1024*1024, &numBytes, 60000);
        }
        printf("\n");
        printf("Dump ROM completed !\n");
        timer_end();
        timer_show();
        myfile = fopen("dump_smd.bin","wb");
        fwrite(BufferROM,1,game_size, myfile);
        fclose(myfile);
        return 0;

    }

    else if ( sms_mode == 0 && Hardwaretype == 3 ) // Automatic Dump Megadrive cartridge in Lock-on mapper mode
    {
        printf("Extra Hardware detected dump in mode : Sega Lock-ON \n");
        printf("Lower Cartridge is : ");
        printf("%.*s\n", 48, (char *)game_name);
        printf("Upper Cartridge is : ");

        address=(0x200100)/2;
        usb_buffer_out[0] = READ_MD;
        usb_buffer_out[1] = address&0xFF ;
        usb_buffer_out[2] = (address&0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4] = 0; // Slow Mode

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
        libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

        memcpy((unsigned char *)dump_name, (unsigned char *)usb_buffer_in+32,32);
        trim((unsigned char *)dump_name, 0);
        printf("%.*s\n",32, (char *)dump_name);
        //trim((unsigned char *)dump_name, 0);

        if(memcmp((unsigned char *)dump_name,"SONIC THE               HEDGEHOG",32) == 0)
        {
            //printf("%.*s\n",32, (char *)game_name);
            game_size=2560*1024;

            BufferROM = (unsigned char*)malloc(game_size);
            address = 0;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
            printf("Starting Dump ...\n");
            res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes, 0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }
            printf("Dump ROM completed !\n");
            myfile = fopen("Sonic & Knuckles + Sonic The Hedgehog.bin","wb");
            fwrite(BufferROM, 1,game_size, myfile);
            fclose(myfile);
        }
        else if(memcmp((unsigned char *)dump_name,"                                ",32) == 0) // Tanglewood use fake header
        {
            //printf("TANGLEWOOD (R)				  ");
            game_size=4096*1024;
            BufferROM = (unsigned char*)malloc(game_size);
            address = 0;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
            printf("Starting Dump ...\n");
            res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes, 0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }
            printf("Dump ROM completed !\n");
            myfile = fopen("Sonic & Knuckles + TANGLEWOOD.bin","wb");
            fwrite(BufferROM, 1,game_size, myfile);
            fclose(myfile);
        }
        else if(memcmp((unsigned char *)dump_name,"SONIC THE             HEDGEHOG 2",32) == 0)
        {
            //printf("%.*s\n",32, (char *)game_name);
            game_size=3328*1024;
            BufferROM = (unsigned char*)malloc(game_size);

            // Dump the SK + S2 part of the ROM
            address = 0;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
            printf("Starting Dump ...\n");
            res = libusb_bulk_transfer(handle, 0x82,BufferROM,3072*1024, &numBytes, 0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }

            // Prepare Lock-On system to map S2 patch at $300000-$3FFFFF:

            address = 0xA130F1/2; // bank 6
            usb_buffer_out[0] = MAPPER_SSF2;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0;
            usb_buffer_out[5]=1;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);

            // Do a simple read maybe needed for init bank or slow down the dumper :D

            address = 0;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
            libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 0);

            // Restart the dump

            address = (3072*1024)/2;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            res = libusb_bulk_transfer(handle, 0x82,BufferROM+3072*1024,256*1024, &numBytes, 60000);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }


            printf("Dump ROM completed !\n");
            myfile = fopen("Sonic & Knuckles + Sonic The Hedgehog2.bin","wb");
            fwrite(BufferROM, 1,3328*1024, myfile);
            fclose(myfile);
        }
        else if(memcmp((unsigned char *)dump_name,"SONIC THE             HEDGEHOG 3",32) == 0)
        {
            //printf("%.*s\n",32, (char *)game_name);
            game_size=4096*1024;
            BufferROM = (unsigned char*)malloc(game_size);

            // Be sure to have Lock-ON disabled
            address = 0xA130F1/2; // bank 6
            usb_buffer_out[0] = MAPPER_SSF2;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0;
            usb_buffer_out[5]=0;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);

            // Do a simple read maybe needed for init bank or slow down the dumper :D

            address = 0;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
            libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 0);

            // Restart the dump

            address = 0;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
            printf("Starting Dump S&K + Sonic 3 ...\n");
            res = libusb_bulk_transfer(handle, 0x82,BufferROM,1024*4096, &numBytes, 0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }

            printf("Dump ROM completed !\n");
            myfile = fopen("Sonic & Knuckles + Sonic The Hedgehog3.bin","wb");
            fwrite(BufferROM, 1,game_size, myfile);
            fclose(myfile);
        }
        else
        {
            //printf("No Cartridge");
            game_size=2048*1024;
            BufferROM = (unsigned char*)malloc(game_size);
            address = 0;
            usb_buffer_out[0] = READ_MD;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
            printf("\nStarting Dump of Sonic & Knuckles\n");
            res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes, 0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }
            printf("Dump ROM completed !\n");
            myfile = fopen("Sonic & Knuckles.bin","wb");
            fwrite(BufferROM, 1,game_size, myfile);
            fclose(myfile);
        }
    }
    else if ( sms_mode == 1 ) //Read in 8 bits mode
    {
        int i=0;
        address=0;
        BufferROM = (unsigned char*)malloc(game_size);

        if (gg_mode == 0 )
            printf("Master System Mode : ROM dump in progress...\n");
        else if (gg_mode == 1 )
            printf("GAME GEAR Mode : ROM dump in progress...\n");

        while (i<game_size)
        {
            usb_buffer_out[0] = READ_SMS;
            usb_buffer_out[1] = address&0xFF ;
            usb_buffer_out[2] = (address&0xFF00)>>8;
            usb_buffer_out[3] = (address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = 0;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,(BufferROM+i),64, &numBytes, 60000);
            address +=64;
            i+=64;
        }
        printf("\n");
        printf("Dump ROM completed !\n");
        timer_end();
        timer_show();
        if (gg_mode == 0)
            myfile = fopen("dump_rom.sms","wb");
        else if (gg_mode == 1 )
            myfile = fopen("dump_rom.gg","wb");
        fwrite(BufferROM, 1,game_size, myfile);
        fclose(myfile);
    }
    return 0;
}

int Read_ROM_Manual(void)
{
    printf("\n");
    printf("Read Mode : Read ROM in manual mode\n");

    printf("Sending command Dump ROM \n");
    printf("Dumping please wait ...\n");
    timer_start();
    switch(dump_rom_size_opts)
    {
    case 0:
        game_size = 32 * 1024;
        break;
    case 1:
        game_size = 64 * 1024;
        break;
    case 2:
        game_size = 128 * 1024;
        break;
    case 3:
        game_size = 256 * 1024;
        break;
    case 4:
        game_size = 512 * 1024;
        break;
    case 5:
        game_size = 1024 * 1024;
        break;
    case 6:
        game_size = 2048 * 1024;
        break;
    case 7:
        game_size = 4096 * 1024;
        break;
    case 8:
        game_size = 8192 * 1024;
        break;
    }

    printf("\n");
    printf("Rom Size (Manual Mode) : %ld Ko \n",game_size/1024);
    BufferROM = (unsigned char*)malloc(game_size);
    // Cleaning ROM Buffer
    for (i=0; i<game_size; i++)
        BufferROM[i]=0x00;

    if(dump_cart_mode_opts==0)						//Game Gear Mode
    {
        int i=0;
        address=0;
        printf("GAME GEAR Mode : ROM dump in progress...\n");

        while (i<game_size)
        {
            usb_buffer_out[0] = READ_SMS;
            usb_buffer_out[1] = address&0xFF ;
            usb_buffer_out[2] = (address&0xFF00)>>8;
            usb_buffer_out[3] = (address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = 0;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,(BufferROM+i),64, &numBytes, 60000);
            address +=64;
            i+=64;
        }

        printf("\n");
        printf("Dump ROM completed !\n");
        timer_end();
        timer_show();
        myfile = fopen("dump_rom.gg","wb");
        fwrite(BufferROM, 1,game_size, myfile);
        fclose(myfile);
    }
    else if(dump_cart_mode_opts==1)						//Mega Drive Mode
    {
        address = 0;
        usb_buffer_out[0] = READ_MD;
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=1;

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
        printf("Mega Drive Mode : ROM dump in progress...\n");
        res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes, 0);
        if (res != 0)
        {
            printf("Error \n");
            return 1;
        }

        printf("\n");
        printf("Dump ROM completed !\n");
        timer_end();
        timer_show();
        myfile = fopen("dump_smd.bin","wb");
        fwrite(BufferROM, 1,game_size, myfile);
        fclose(myfile);
    }
    else if(dump_cart_mode_opts==2)					//Master System Mode
    {
        address = 0;
        int i=0;
        printf("Master System Mode : ROM dump in progress...\n");
        while (i<game_size)
        {
            usb_buffer_out[0] = READ_SMS;
            usb_buffer_out[1] = address&0xFF ;
            usb_buffer_out[2] = (address&0xFF00)>>8;
            usb_buffer_out[3] = (address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = 0;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,(BufferROM+i),64, &numBytes, 60000);
            address +=64;
            i+=64;
        }
        printf("\n");
        printf("Dump ROM completed !\n");
        timer_end();
        timer_show();
        myfile = fopen("dump_sms.sms","wb");
        fwrite(BufferROM, 1,game_size, myfile);
        fclose(myfile);
    }
    return 0;
}

int Read_ROM_Bankswitch(void)
{
    printf("\n");
    printf("Read Mode : Read ROM in mode : Bankswitch SSF2 \n");
    i=0;
    while (i<8)
    {
        usb_buffer_out[0] = READ_MD;
        usb_buffer_out[1] = address&0xFF ;
        usb_buffer_out[2] = (address&0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4] = 0; // Slow Mode

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
        libusb_bulk_transfer(handle, 0x82,buffer_header+(64*i),64, &numBytes, 60000);
        address+=32;
        i++;
    }

    if(memcmp((unsigned char *)buffer_header,"SEGA",4) == 0)
    {
        printf("\n");
        printf("Megadrive/Genesis/32X cartridge detected!\n");
        printf("\n");
        printf(" --- HEADER ---\n");
        memcpy((unsigned char *)dump_name, (unsigned char *)buffer_header+32, 48);
        trim((unsigned char *)dump_name, 0);
        printf(" Domestic: %.*s\n", 48, (char *)game_name);
        memcpy((unsigned char *)dump_name, (unsigned char *)buffer_header+80, 48);
        trim((unsigned char *)dump_name, 0);

        printf(" International: %.*s\n", 48, game_name);
        printf(" Release date: %.*s\n", 16, buffer_header+0x10);
        printf(" Version: %.*s\n", 14, buffer_header+0x80);
        memcpy((unsigned char *)region, (unsigned char *)buffer_header +0xF0, 4);
        for(i=0; i<4; i++)
        {
            if(region[i]==0x20)
            {
                game_region = (char *)malloc(i);
                memcpy((unsigned char *)game_region, (unsigned char *)buffer_header +0xF0, i);
                game_region[i] = '\0';
                break;
            }
        }

        if(game_region[0]=='0')
        {
            game_region = (char *)malloc(4);
            memcpy((char *)game_region, (char *)unk, 3);
            game_region[3] = '\0';
        }

        printf(" Region: %s\n", game_region);
        checksum_header = (buffer_header[0x8E]<<8) | buffer_header[0x8F];
        printf(" Checksum: %X\n", checksum_header);
        game_size = 1 + ((buffer_header[0xA4]<<24) | (buffer_header[0xA5]<<16) | (buffer_header[0xA6]<<8) | buffer_header[0xA7])/1024;
        printf(" Game size: %dKB\n", game_size);
    }

    // Search checksum cartridge in Custom Hardware games csv table
    i=0;
    for (i = 0; i < chksm_text_values_count ; i++)
    {
        strncpy(txt_csv_chksm,chksm_text_values[i],4);
        //printf(" txt chksm value : %s \n",txt_csv_chksm);
        csv_chksm = (unsigned short)strtol(txt_csv_chksm, NULL, 16);
        if ( checksum_header == csv_chksm  )
        {
            Index_chksm = i;
            printf("\n");
            printf("Found game in extra CSV Gamelist  \n");
            printf("Position in csv table %d \n",i);
            strncpy(txt_csv_game_size,chksm_text_values[i]+5,4);
            txt_csv_game_size[4] = '\0'; // Null-terminate the output string
            //printf(" txt game size : %s \n",txt_csv_game_size);
            csv_game_size = (unsigned char)strtol(txt_csv_game_size, NULL, 10);
            //printf(" CSV Game Size  %d \n",csv_game_size);
            game_size=1024*csv_game_size;
            printf("ROM Size from CSV is %ld Ko \n",game_size);
        }
    }
    NumberOfBank = game_size/512;
    //printf("Game Size is %ld Ko \n",game_size);
    printf("Number of Banks is %d \n",NumberOfBank);
    printf("Bank Size is 512 Ko  \n");

    game_size = game_size * 1024;
    BufferROM = (unsigned char*)malloc(game_size);
    // Cleaning ROM Buffer
    for (i=0; i<game_size; i++)
        BufferROM[i]=0x00;

    // Dump the first 4MB of the ROM as fast as possible
    printf("Bankswith bank O-7 to $080000 - $3FFFFF \n");

    printf("Dumping please wait ...\n");
    timer_start();
    address = 0;
    i=0;

    usb_buffer_out[0] = READ_MD;
    usb_buffer_out[1]=address & 0xFF;
    usb_buffer_out[2]=(address & 0xFF00)>>8;
    usb_buffer_out[3]=(address & 0xFF0000)>>16;
    usb_buffer_out[4]=1;

    libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
    printf("ROM dump in progress...\n");
    res = libusb_bulk_transfer(handle, 0x82,BufferROM,4096*1024, &numBytes,0);
    if (res != 0)
    {
        printf("Error \n");
        return 1;
    }

    ActualBank = 8;
    offset = 4096 - 1024;

    while ( offset != (game_size/1024)-1024)
    {
        printf("Bankswith bank %d - %d to $200000 - $2FFFFF \n",ActualBank,ActualBank+1);
        printf("Dumping please wait ...\n");

        address = 0xA130F9/2; // bank 4
        usb_buffer_out[0] = MAPPER_SSF2;
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=0;
        usb_buffer_out[5]=ActualBank;

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);

        // Send 0x09 to the bank 7
        address = 0xA130FB/2; // bank 5
        usb_buffer_out[0] = MAPPER_SSF2;
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=0;
        usb_buffer_out[5]=ActualBank+1;

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
        address = (2048*1024)/2;
        // Dump lastMB
        // Do a simple read maybe needed for init bank or slow down the dumper :D
        usb_buffer_out[0] = READ_MD;
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=0;

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0);
        libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 0);

        // Re-start the dump
        address = (2048*1024)/2;
        usb_buffer_out[0] = READ_MD;
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=1;

        offset = offset + 1024;
        ActualBank = ActualBank +2;
        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
        res = libusb_bulk_transfer(handle, 0x82,BufferROM+offset*1024,1024*1024, &numBytes, 60000);
    }
    printf("\n");
    printf("Dump ROM completed !\n");
    timer_end();
    timer_show();
    myfile = fopen("dump_smd.bin","wb");
    fwrite(BufferROM,1,game_size, myfile);
    fclose(myfile);
    return 0;
}

int Read_RAM_Auto(void)
{
    printf("Read Mode Auto: Read Save Data\n");
    save_size *= 1024;
    // if (save_size < 8*1024){save_size=8*1024;}  // SRAM chip can't be low as 8 Ko

    BufferROM = (unsigned char*)malloc(save_size); // raw buffer
    BufferSAVE = (unsigned char*)malloc((save_size*2)); // raw in 16bit format
    for (i=0; i<(save_size*2); i++)
        BufferSAVE[i]=0x00;

    usb_buffer_out[0] = READ_MD_SAVE;
    address=(save_address/2);
    i=0;
    while ( i< save_size)
    {
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=0;
        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
        libusb_bulk_transfer(handle, 0x82,(BufferROM+i),64, &numBytes, 60000);
        address +=64; //next adr
        i+=64;
        fflush(stdout);
    }

    i=0;
    j=0;
    myfile = fopen("raw.srm","wb");
    fwrite(BufferROM,1,save_size, myfile);

    for (i=0; i<save_size; i++)
    {
        j=j+1;
        BufferSAVE[i+j]=BufferROM[i];
    }

    myfile = fopen("dump_smd.srm","wb");
    fwrite(BufferSAVE,1,save_size*2, myfile);
    fclose(myfile);
    printf("\n");
    printf("Save Data completed !\n");
    timer_end();
    timer_show();
    return 0;
}

int Read_RAM_Bankswitch(void)
{
    printf("Read Mode Bankswitch: Read Save Data\n");
    printf("TODO !...\n");
}

int Read_RAM_Manual(void)
{
    printf("Read Mode Manual : Read Save Data\n");
    printf("Reading in progress...\n");
    printf("%ld",dump_sram_size_opts);
    timer_start();
    if(dump_sram_size_opts==0)
        save_size = 8192;
    else if(dump_sram_size_opts==1)
        save_size = 32768;
    else
        save_size *= 1024;

    BufferROM = (unsigned char*)malloc(save_size); // raw buffer
    BufferSAVE = (unsigned char*)malloc((save_size*2)); // raw in 16bit format

    for (i=0; i<(save_size*2); i++)
        BufferSAVE[i]=0x00;

    usb_buffer_out[0] = READ_MD_SAVE;
    address=(save_address/2);
    i=0;
    while ( i< save_size)
    {
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=0;
        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
        libusb_bulk_transfer(handle, 0x82,(BufferROM+i),64, &numBytes, 60000);
        address +=64; //next adr
        i+=64;
        fflush(stdout);
    }
    i=0;
    j=0;
    myfile = fopen("raw.srm","wb");
    fwrite(BufferROM,1,save_size, myfile);

    for (i=0; i<save_size; i++)
    {
        j=j+1;
        BufferSAVE[i+j]=BufferROM[i];
    }

    myfile = fopen("dump_smd.srm","wb");
    fwrite(BufferSAVE,1,save_size*2, myfile);
    fclose(myfile);
    printf("\n");
    printf("Save Data completed !\n");
    timer_end();
    timer_show();
    return 0;
}
