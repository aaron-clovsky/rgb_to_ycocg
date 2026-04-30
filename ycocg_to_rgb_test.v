`timescale 1ns / 1ps

module ycocg_to_rgb_test;

    integer iY, iCo, iCg;

    reg  [7:0] Y;
    reg  [7:0] Co;
    reg  [7:0] Cg;
    wire [7:0] R;
    wire [7:0] G;
    wire [7:0] B;

    ycocg_to_rgb test (
        .Y(Y),
        .Co(Co),
        .Cg(Cg),
        .R(R),
        .G(G),
        .B(B)
    );

    initial begin
        for (iY = 0; iY <= 255; iY++) begin
            for (iCo = 0; iCo <= 255; iCo++) begin
                for (iCg = 0; iCg <= 255; iCg++) begin
                    Y = iY;
                    Co = iCo;
                    Cg = iCg;
                    #1
                    $display("%0d,%0d,%0d,%0d,%0d,%0d",Y,Co,Cg,R,G,B);
                end
            end
        end

        $finish;
    end

endmodule
