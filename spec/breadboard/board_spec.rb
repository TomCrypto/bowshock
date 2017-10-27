require_relative 'board'

describe Breadboard, hardware: true do
  subject(:board) { described_class.new params, links }

  describe 'digital_io' do
    before { board.upload 'bin/test-firmware.bin' }

    context 'with the input pin pulled down' do
      let(:params) { { termination: :pulldown } }

      it 'is low when driven low' do
        expect(board.response.state_when_driven_low).to eq 'low'
      end

      it 'is low when not driven' do
        expect(board.response.state_when_not_driven).to eq 'low'
      end

      it 'is high when driven high' do
        expect(board.response.state_when_driven_high).to eq 'high'
      end

      it 'is low when not driven again' do
        expect(board.response.state_when_not_driven_again).to eq 'low'
      end
    end

    context 'with the input pin pulled up' do
      let(:params) { { termination: :pullup } }

      it 'is low when driven low' do
        expect(board.response.state_when_driven_low).to eq 'low'
      end

      it 'is high when not driven' do
        expect(board.response.state_when_not_driven).to eq 'high'
      end

      it 'is high when driven high' do
        expect(board.response.state_when_driven_high).to eq 'high'
      end

      it 'is high when not driven again' do
        expect(board.response.state_when_not_driven_again).to eq 'high'
      end
    end

    context 'with the input pin in repeater mode' do
      let(:params) { { termination: :repeater } }

      it 'is low when driven low' do
        expect(board.response.state_when_driven_low).to eq 'low'
      end

      it 'is low when not driven' do
        expect(board.response.state_when_not_driven).to eq 'low'
      end

      it 'is high when driven high' do
        expect(board.response.state_when_driven_high).to eq 'high'
      end

      it 'is high when not driven again' do
        expect(board.response.state_when_not_driven_again).to eq 'high'
      end
    end
  end
end
