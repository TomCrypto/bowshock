require_relative 'board'

describe LPC1100::MMIO, hardware: true do
  subject(:board) { described_class.new params, links }

  describe 'rtl::mmio<rtl::u32>' do
    before { board.upload 'bin/lpc1100-mmio-firmware.bin' }

    let(:params) do
      {
        operation: operation,
        initial_value: initial_value,
        argument: argument
      }
    end

    # Note the mask and bit parameters below are hardcoded in the firmware, as
    # they are template arguments. Changing them from here will have no effect
    # and they are only shown here to make the spec easier to interpret.

    describe 'clear<MASK>' do
      let(:operation)       { :masked_clear }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:mask)            { 0b11010101111110010011101100011011 }
      let(:argument)        { nil }
      let(:expected_result) { 0b00000010000001001100000011100100 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'clear' do
      let(:operation)       { :clear }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:argument)        { nil }
      let(:expected_result) { 0b00000000000000000000000000000000 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'set<MASK>' do
      let(:operation)       { :masked_set }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:mask)            { 0b11010101111110010011101100011011 }
      let(:argument)        { nil }
      let(:expected_result) { 0b11010111111111011111101111111111 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'set' do
      let(:operation)       { :set }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:argument)        { nil }
      let(:expected_result) { 0b11111111111111111111111111111111 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'toggle<MASK>' do
      let(:operation)       { :toggle }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:mask)            { 0b11010101111110010011101100011011 }
      let(:argument)        { nil }
      let(:expected_result) { 0b10000111010001011111000111110110 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'write<MASK>' do
      context 'in bounds of mask' do
        let(:operation)       { :masked_write }
        let(:initial_value)   { 0b01010010101111001100101011101101 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { 0b10000100111010010000100000010000 }
        let(:expected_result) { 0b10000110111011011100100011110100 }

        it 'returns the correct result' do
          expect(board.response.value).to eq expected_result
        end
      end

      context 'out of bounds of mask' do
        let(:operation)       { :masked_write }
        let(:initial_value)   { 0b01010010101111001100101011101101 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { 0b10000101100101010010101000001001 }

        it 'asserts' do
          expect { board.response }.to raise_error(
            AssertionError, /attempted to write bits outside mask/
          )
        end
      end
    end

    describe 'safe_write<MASK>' do
      let(:operation)       { :safe_write }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:mask)            { 0b11010101111110010011101100011011 }
      let(:argument)        { 0b10000101100101010010101000001001 }
      let(:expected_result) { 0b10000111100101011110101011101101 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'write' do
      let(:operation)       { :write }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:argument)        { 0b11010101011010101010100100101101 }
      let(:expected_result) { 0b11010101011010101010100100101101 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'read<MASK>' do
      let(:operation)       { :read }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:mask)            { 0b11010101111110010011101100011011 }
      let(:argument)        { nil }
      let(:expected_result) { 0b01010000101110000000101000001001 }

      it 'returns the correct result' do
        expect(board.response.read).to eq expected_result
      end
    end

    describe 'any<MASK>' do
      context 'with some bits in the mask set' do
        let(:operation)       { :any }
        let(:initial_value)   { 0b01010010101111001100101011101101 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { nil }

        it 'returns the correct result' do
          expect(board.response.bit).to be true
        end
      end

      context 'with no bits in the mask set' do
        let(:operation)       { :any }
        let(:initial_value)   { 0b00100010000001101100010000000100 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { nil }

        it 'returns the correct result' do
          expect(board.response.bit).to be false
        end
      end
    end

    describe 'all<MASK>' do
      context 'with all bits in the mask set' do
        let(:operation)       { :all }
        let(:initial_value)   { 0b11110111111110111111101101011111 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { nil }

        it 'returns the correct result' do
          expect(board.response.bit).to be true
        end
      end

      context 'with some bits in the mask set' do
        let(:operation)       { :all }
        let(:initial_value)   { 0b01010010101111001100101011101101 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { nil }

        it 'returns the correct result' do
          expect(board.response.bit).to be false
        end
      end

      context 'with no bits in the mask set' do
        let(:operation)       { :all }
        let(:initial_value)   { 0b00100010000001101100010000000100 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { nil }

        it 'returns the correct result' do
          expect(board.response.bit).to be false
        end
      end
    end

    describe 'none<MASK>' do
      context 'with some bits in the mask set' do
        let(:operation)       { :none }
        let(:initial_value)   { 0b01010010101111001100101011101101 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { nil }

        it 'returns the correct result' do
          expect(board.response.bit).to be false
        end
      end

      context 'with no bits in the mask set' do
        let(:operation)       { :none }
        let(:initial_value)   { 0b00100010000001101100010000000100 }
        let(:mask)            { 0b11010101111110010011101100011011 }
        let(:argument)        { nil }

        it 'returns the correct result' do
          expect(board.response.bit).to be true
        end
      end
    end

    describe 'clear_bit<BIT>' do
      let(:operation)       { :clear_bit }
      let(:initial_value)   { 0b01010010101111001100101011101101 }
      let(:argument)        { nil } #       ^ bit 19 is affected
      let(:expected_result) { 0b01010010101101001100101011101101 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'set_bit<BIT>' do
      let(:operation)       { :set_bit }
      let(:initial_value)   { 0b01010010101101001100101011101101 }
      let(:argument)        { nil } #       ^ bit 19 is affected
      let(:expected_result) { 0b01010010101111001100101011101101 }

      it 'returns the correct result' do
        expect(board.response.value).to eq expected_result
      end
    end

    describe 'toggle_bit<BIT>' do
      context 'with the target bit not set' do
        let(:operation)       { :toggle_bit }
        let(:initial_value)   { 0b01010010101101001100101011101101 }
        let(:argument)        { nil } #       ^ bit 19 is affected
        let(:expected_result) { 0b01010010101111001100101011101101 }

        it 'returns the correct result' do
          expect(board.response.value).to eq expected_result
        end
      end

      context 'with the target bit set' do
        let(:operation)       { :toggle_bit }
        let(:initial_value)   { 0b01010010101111001100101011101101 }
        let(:argument)        { nil } #       ^ bit 19 is affected
        let(:expected_result) { 0b01010010101101001100101011101101 }

        it 'returns the correct result' do
          expect(board.response.value).to eq expected_result
        end
      end
    end

    describe 'read_bit<BIT>' do
      context 'with the target bit not set' do
        let(:operation)       { :read_bit }
        let(:initial_value)   { 0b01010010101101001100101011101101 }
        let(:argument)        { nil } #       ^ bit 19 is affected

        it 'returns the correct result' do
          expect(board.response.bit).to be false
        end
      end

      context 'with the target bit set' do
        let(:operation)       { :read_bit }
        let(:initial_value)   { 0b01010010101111001100101011101101 }
        let(:argument)        { nil } #       ^ bit 19 is affected

        it 'returns the correct result' do
          expect(board.response.bit).to be true
        end
      end
    end
  end
end
