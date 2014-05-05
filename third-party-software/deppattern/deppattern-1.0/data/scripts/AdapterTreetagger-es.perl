#!/usr/bin/perl -w

##Entrada: saida do TreeTagger portugu�s
##Saida: entrada do parsingCascataByRegularExpressions.perl


#$UpperCase = "[A-Z\301\311\315\323\332\307\303\325\302\312\324\300\310�]" ;
#$LowerCase = "[a-z��������������]" ;

## Lexemas gramaticais especiais:
$AUX="(have|avoir|haber|haver)";
$COP="(be|�tre|ser)";

$Pos=0;
while (<>) {
   chop($_);

   if ($_ eq "") {
      next
   }

  else {

     ($token, $tag, $lemma) = split("\t", $_);

   ##correc�oes ad hoc de problemmas de etiqueta�ao:
  # if ($lemma =~ /suivre\|�tre$/)   {
  #         $lemma = "�tre"
  # }

   ## o lemma "del" passa a "de" e "al" a "a":
   if ($tag eq "PDEL")  {
       $lemma = "de";
       $tag = "PREP"
   }
   elsif ($tag eq "PAL")  {
       $lemma = "a";
       $tag = "PREP"
   }
  ##negacao NEG passa a ADV, NMEA e NMON passa a NC
   elsif ($tag eq "NEG" ) {
          $tag = "ADV"
   }
   elsif  ($tag eq "NMEA" || $tag eq "NMON" ) {
          $tag = "NC"
   }
  ## pronome SE
   elsif ($tag eq "SE" ) {
          $tag = "PPC"
   }


   #se temos um poss�vel nome pr�prio (desconhecido ou composto), colocar a forma no lemma (para conservar a maiuscula)
   if ( ($token =~ /\&/) || ($lemma =~ /<unknown>/) ) {
         $lemma = $token;
   }
#   elsif ( ($tag =~ /^NOM/) && ($token =~ /^$UpperCase/) && ($lemma =~ /^$LowerCase/) ) {
 #        $lemma = $token;
 #  }

 

   ##pronomes:
   
   if ($tag =~ /(^PP[CX]|^REL|^INT)/) {
      $Exp{"lemma"} = $lemma;
      $Exp{"token"} = $token;
      $Exp{"tag"} =  "PRO";
      if ($tag =~ /^PP/) {
        $Exp{"type"} = "P";  
      }
      
      elsif  ($tag =~ /^REL/) {
      
        $Exp{"type"} = "R";
     }
     elsif  ($tag =~ /^INT/) {
      
        $Exp{"type"} = "T";
     }
     $Exp{"person"} = 0;
     $Exp{"gender"} = 0;
     $Exp{"number"} = 0;
     $Exp{"case"} = 0;    
     $Exp{"possessor"} = 0;    
     $Exp{"politeness"} = 0;
 
   } 
      
     ##conjunctions:
   elsif ($tag =~ /(^CC|^CS|^CQ)/) {
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "CONJ";
       if ($tag =~ /^CC/) {
          $Exp{"type"} = "C";
      }
      else {
         $Exp{"type"} =  "S";
      }      
    }
  
    ##interjections:
    elsif ($tag =~ /^ITJN/) {
       (@tmp) = split ("", $tag);
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "I";
       $Exp{"type"} = 0;
      
    }

   ##numbers
   elsif ($tag =~  /^CARD/) {
      # $lemma = "\@card\@";
       (@tmp) = split ("", $tag);
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "CARD";
       $Exp{"number"} = "P";
       $Exp{"person"} = 0;         
       $Exp{"gender"} = 0;         
       
   }


    elsif ($tag =~ /(^ADJ|^ORD)/) {
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "ADJ";
       $Exp{"type"} = 0;
       $Exp{"degree"} = 0;
       $Exp{"gender"} = 0;
       $Exp{"number"} = 0;
       $Exp{"function"} = 0;
   }


   elsif ($tag =~ /^ADV/) {
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "ADV";
       $Exp{"type"} = 0;
     
   }


   elsif ($tag =~ /^PREP/) {
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "PRP";
       $Exp{"type"} = 0;
      
   }


   elsif ($tag =~ /^N[CP]/) {
       (@tmp) = split ("", $tag);
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "NOUN";
       if ($tag =~ /^NC/) {
          $Exp{"type"} = "C";
       }
       else {
           $Exp{"type"} = "P"
       }
       $Exp{"gender"} = 0;
       $Exp{"number"} = 0;
       $Exp{"person"} = 3;

   }

   elsif ($tag =~ /(^ART|^DM|^PPO|^QU)/) {
       (@tmp) = split ("", $tag);
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "DT";
       if  ($tag =~ /^ART/) {
         $Exp{"type"} = "A";
      }
      elsif  ($tag =~ /^DM/) {
         $Exp{"type"} = "D";
      }
      elsif  ($tag =~ /^QU/) {
         $Exp{"type"} = "I";
      }
      elsif  ($tag =~ /^PPO/) {
         $Exp{"type"} = "P";
      }
      else {
         $Exp{"type"} = 0;
      }

       $Exp{"type"} = 0;
       $Exp{"person"} = 0;
       $Exp{"gender"} = 0;
       $Exp{"number"} = 0;
       $Exp{"possessor"} = 0;
        
   }

  ##mudar tags nos verbos:
   elsif ($tag =~ /^V/) {

       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "VERB";
       if  ($lemma =~ /^$AUX$/) {
        $Exp{"type"} = "A";
       }
       elsif ($lemma =~ /^$COP$/) {
          $Exp{"type"} = "S";
       }
       else {
          $Exp{"type"} = 0;
       }
       if ($token =~ /[ai]d[ao](s?)$/) {
        $Exp{"mode"} = "P";
       }
       elsif ($token =~ /[aei]r$/) {
        $Exp{"mode"} = "N";
       }
       elsif ($token =~ /[aei]ndo$/) {
        $Exp{"mode"} = "G";
       }
        else {
          $Exp{"mode"} = 0;
       }

       $Exp{"tense"} = 0;
       $Exp{"person"} = 0;
       $Exp{"number"} = 0;
       $Exp{"gender"} = 0;

   }


     ##simbolos puntuacao:
 #    elsif  ($token eq "!")  {
       
   #    $Exp{"lemma"} = $lemma;
  #     $Exp{"token"} = $token;
  #     $Exp{"tag"} =  "Fat";
 #  }

   elsif  ($token eq "�")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Faa";
   }

   elsif  ($token eq ",")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fc";
   }
   elsif  ($token eq "\[")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fca";
   }
   elsif  ($token eq "\]")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fct";
   }
   elsif  ($token eq ":")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fd";
   } 
   elsif  ($token eq "\"")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fe";
   }
   elsif  ($token eq "-")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fg";
   }
   elsif  ($token eq "\/")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fh";
   }
   elsif  ($token eq "�")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fia";
   } 
 #   elsif  ($token eq "?")  {
       
 #      $Exp{"lemma"} = $lemma;
 #      $Exp{"token"} = $token;
 #      $Exp{"tag"} =  "Flt";
 #  } 
   elsif  ($token eq "{")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fla";
   } 
    elsif  ($token eq "}")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Flt";
   } 
   elsif  ($token eq "(")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fpa";
   } 
   elsif  ($token eq ")")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fpt";
   } 
   elsif  ($token eq "\�")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fra";
   } 
   elsif  ($token eq "\�")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Frc";
   } 
   elsif  ($token eq "\.\.\.")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fs";
   } 
   elsif  ($token eq "%")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Ft";
   }  
   elsif  ($token eq "\;")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fx";
   } 

   elsif  ($token eq "[+-=]")  {
       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "Fz";
   }

    ##final de frase (.)
    elsif ($token =~ /^\.$|^\?$|^\!$/) {
   # print STDERR "--$lemma -- $tag\n";       
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "SENT";
       $Exp{"pos"} =  $Pos;
       $Pos=0;
   }
   else {
      $Exp{"lemma"} = $lemma;
      $Exp{"token"} = $token;
      $Exp{"tag"} =  $tag;
   }

  
   

   ##Colocar a posi�ao em todos:
    if ($token !~ /^\.$|^\?$|^\!$/) { 
       $Exp{"pos"} =  $Pos;
       $Pos++;
    }

  # if   ($Exp{"tag"} =~ /^Fra$|^Frc$|Fe$/) {

 #  }
  # else {
       print "$token\t" ;
       foreach $attrib (sort keys %Exp) {
	   print "$attrib:$Exp{$attrib}|" ;
           delete $Exp{$attrib};
       }
       print "\n";
 #  }

 }
}



