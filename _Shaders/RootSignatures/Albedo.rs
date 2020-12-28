#define RootSignatureDef "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )," \
                         "DescriptorTable(" \
                            "CBV(b0, numDescriptors=2, flags = DESCRIPTORS_VOLATILE)," \
                            "SRV(t0, numDescriptors=1, flags = DESCRIPTORS_VOLATILE)," \
                            "visibility=SHADER_VISIBILITY_ALL" \
                         ")," \
                         "DescriptorTable(Sampler(s0, numDescriptors=1, flags = DESCRIPTORS_VOLATILE), visibility=SHADER_VISIBILITY_PIXEL)"