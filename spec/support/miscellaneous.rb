# Various helper methods or things missing from Ruby that I find useful

class String
  def to_b
    case self
    when 'false'
      false
    when 'true'
      true
    else
      raise 'not a boolean string'
    end
  end
end
