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
   if ($lemma =~ /suivre\|�tre$/)   {
           $lemma = "�tre"
   }

   ## o lemma "du" passa a "de" e "au" a "�":
   if ($lemma eq "du" || $lemma eq "des") {
       $lemma = "de"
   }
   elsif ($lemma eq "au" || $lemma eq "aux" ) {
      $lemma = "�"
   }

   elsif ($lemma eq "ce" ) {
          $token = "DET:DEM"
   }
   elsif ($lemma eq "quelque" ) {
          $token = "DET:IND"
   }

   #se temos um poss�vel nome pr�prio (desconhecido ou composto), colocar a forma no lemma (para conservar a maiuscula)
   if ( ($token =~ /\&/) || ($lemma =~ /<unknown>/) ) {
         $lemma = $token;
   }
#   elsif ( ($tag =~ /^NOM/) && ($token =~ /^$UpperCase/) && ($lemma =~ /^$LowerCase/) ) {
 #        $lemma = $token;
 #  }

 

   ##pronomes:
   
   if ($tag =~ /^PRO/) {
      $Exp{"lemma"} = $lemma;
      $Exp{"token"} = $token;
      $Exp{"tag"} =  "PRO";
      if  ($tag =~ /^PRO:PER/) {
         $Exp{"type"} = "P";
      }
      elsif  ($tag =~ /^PRO:DEM/) {
         $Exp{"type"} = "D";
      }
      elsif  ($tag =~ /^PRO:IND/) {
         $Exp{"type"} = "I";
      }
      elsif  ($tag =~ /^PRO:POS/) {
         $Exp{"type"} = "X";
      }
      elsif  ($tag =~ /^PRO:REL/) {
         $Exp{"type"} = "R";
      }
      else {
         $Exp{"type"} = 0;
      }
      $Exp{"person"} = 0;
      $Exp{"gender"} = 0;
      $Exp{"number"} = 0;
      $Exp{"case"} = 0;    
      $Exp{"possessor"} = 0;    
      $Exp{"politeness"} = 0;

   } 
      
     ##conjunctions:
    elsif ($tag =~ /^KON/) {
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "C";
       if ($lemma =~ /(^que$|^qui$)/) {
          $Exp{"type"} = "S";
      }
       else {
         $Exp{"type"} =  0;
       }      
    }
  
    ##interjections:
    elsif ($tag =~ /^INT/) {
       (@tmp) = split ("", $tag);
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "I";
       $Exp{"type"} = 0;
      
    }

   ##numbers
   elsif ($tag =~  /^NUM/) {
      # $lemma = "\@card\@";
       (@tmp) = split ("", $tag);
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "CARD";
       $Exp{"number"} = "P";
       $Exp{"person"} = 0;         
       $Exp{"gender"} = 0;         
       
   }


    elsif ($tag =~ /^ADJ/) {
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


   elsif ($tag =~ /^PRP/) {
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "PRP";
       $Exp{"type"} = 0;
      
   }


   elsif ($tag =~ /^N[OA]/) {
       (@tmp) = split ("", $tag);
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "NOUN";
       if ($tag =~ /^NAM/) {
          $Exp{"type"} = "P";
       }
       else {
           $Exp{"type"} = "C"
       }
       $Exp{"gender"} = 0;
       $Exp{"number"} = 0;
       $Exp{"person"} = 3;

   }

   elsif ($tag =~ /^DET/) {
       (@tmp) = split ("", $tag);
       $Exp{"lemma"} = $lemma;
       $Exp{"token"} = $token;
       $Exp{"tag"} =  "DT";
       if  ($tag =~ /^DET:A/) {
         $Exp{"type"} = "A";
      }
      elsif  ($tag =~ /^DET:DEM/) {
         $Exp{"type"} = "D";
      }
      elsif  ($tag =~ /^DET:IND/) {
         $Exp{"type"} = "I";
      }
      elsif  ($tag =~ /^DET:POS/) {
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
       if ($tag =~ /^VER:pper/) {
        $Exp{"mode"} = "P";
       }
       elsif ($tag =~ /^VER:infi/) {
        $Exp{"mode"} = "N";
       }
       elsif ($tag =~ /^VER:ppre/) {
        $Exp{"mode"} = "G";
       }
       elsif ($tag =~ /^VER:sub/) {
        $Exp{"mode"} = "S";
       }
       elsif ($tag =~ /^VER:impe/) {
        $Exp{"mode"} = "M";
       } 
       else {
          $Exp{"mode"} = 0; 
       }

       if ($tag =~ /^VER:pres/) {
        $Exp{"tense"} = "P";
       }
       elsif ($tag =~ /^VER:impf/) {
        $Exp{"tense"} = "I";
       }
       elsif ($tag =~ /^VER:futu/) {
        $Exp{"tense"} = "F";
       }
       elsif ($tag =~ /^VER:simp/) {
        $Exp{"tense"} = "P";
       }
       elsif ($tag =~ /^VER:cond/) {
        $Exp{"tense"} = "C";
       }
       else {
         $Exp{"tense"} = 0;
       }
       $Exp{"person"} = 0;
       $Exp{"number"} = 0;
       $Exp{"gender"} = 0;

   }

     ##simbolos puntuacao:
  #   elsif  ($token eq "!")  {
       
  #     $Exp{"lemma"} = $lemma;
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
  #  elsif  ($token eq "?")  {
       
  #     $Exp{"lemma"} = $lemma;
  #     $Exp{"token"} = $token;
   #    $Exp{"tag"} =  "Flt";
  # } 
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
  #  print STDERR "--$lemma -- $tag\n";       
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
 #  else {
       print "$token\t" ;
       foreach $attrib (sort keys %Exp) {
	   print "$attrib:$Exp{$attrib}|" ;
           delete $Exp{$attrib};
       }
       print "\n";
  # }

 }
}


##as conjun�oes seguem a ser CS (subord) e CC (coordenada)
